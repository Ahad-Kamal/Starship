#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"

Entity::Entity( Game* owner, Vec2 const& startPos )
	: m_game( owner )
	, m_position( startPos )
{
}

Entity::~Entity()
{

}

void Entity::DebugRender() const
{
	Vec2 forwardNormal = GetForwardNormal();
	Vec2 leftNormal = forwardNormal.GetRotatedBy90Degrees();

	Vec2 forwardVector = m_position + (forwardNormal * m_physicsRadius );
	Vec2 leftVector = m_position + ( leftNormal * m_physicsRadius );

	Vec2 worldCenter = Vec2( WORLD_CENTER_X, WORLD_CENTER_Y );

	DebugDrawLine( worldCenter, m_position, DEBUG_LINE_THICKNESS, Rgba8( 50, 50, 50, 255 ) );
	DebugDrawRing( m_position, m_physicsRadius, DEBUG_LINE_THICKNESS, Rgba8( 0, 255, 255 ) );
	DebugDrawRing( m_position, m_cosmeticRadius, DEBUG_LINE_THICKNESS, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( m_position, forwardVector, DEBUG_LINE_THICKNESS, Rgba8( 255, 0, 0) );
	DebugDrawLine( m_position, leftVector, DEBUG_LINE_THICKNESS, Rgba8( 0, 255, 0 ) );
	DebugDrawLine( m_position, m_position + m_velocity, DEBUG_LINE_THICKNESS, Rgba8( 255, 255, 0 ) );
}

void Entity::TakeDamage( int damage )
{
	m_health -= damage;

	if( m_health <= 0 )
	{
		Die();
	}
}

void Entity::Die()
{
	m_isDead = true;
	m_isGarbage = true;

	int count = g_rng->RollRandomIntInRange( 10, 30 );
	m_game->SpawnNewDebrisCluster( count, m_position, m_velocity * 0.5f, GetForwardNormal(), m_color, 1.f );

	g_engine->m_audio->StartSound( audio_enemyExplosion, false, 0.25f );
}

bool Entity::IsAlive() const
{
	if( this == nullptr )
	{
		return false;
	}

	return !m_isDead;
}

bool Entity::IsOffScreen() const
{
	if( m_position.x >= WORLD_SIZE_X + ASTEROID_COSMETIC_RADIUS )
	{
		return true;
	}
	else if( m_position.y >= WORLD_SIZE_Y + ASTEROID_COSMETIC_RADIUS )
	{
		return true;
	}
	else if( m_position.x <= -ASTEROID_COSMETIC_RADIUS )
	{
		return true;
	}
	else if( m_position. y <= -ASTEROID_COSMETIC_RADIUS )
	{
		return true;
	}

	return false;
}

void Entity::BounceOffWalls()
{
	if( m_position.x > WORLD_SIZE_X - ASTEROID_COSMETIC_RADIUS )
	{
		m_velocity.x *= -1;
	}
	else if( m_position.y > WORLD_SIZE_Y - ASTEROID_COSMETIC_RADIUS )
	{
		m_velocity.y *= -1;
	}
	else if( m_position.x < ASTEROID_COSMETIC_RADIUS )
	{
		m_velocity.x *= -1;
	}
	else if( m_position.y < ASTEROID_COSMETIC_RADIUS )
	{
		m_velocity.y *= -1;
	}
}

void Entity::WrapAroundScreen()
{
	if( m_position.x < 0.f - m_cosmeticRadius )
	{
		m_position.x = WORLD_SIZE_X + m_cosmeticRadius;
	}
	else if( m_position.x > WORLD_SIZE_X + m_cosmeticRadius )
	{
		m_position.x = 0.f - m_cosmeticRadius;
	}
	else if( m_position.y < 0.f - m_cosmeticRadius )
	{
		m_position.y = WORLD_SIZE_Y + m_cosmeticRadius;
	}
	else if( m_position.y > WORLD_SIZE_Y + m_cosmeticRadius )
	{
		m_position.y = 0.f - m_cosmeticRadius;
	}
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) );
}

void Entity::ResetFireTick()
{
	m_fireTick = FIRE_DAMAGE_TICK;
}

void Entity::ResetSlowTimer()
{
	m_iceSlowTimer = ICE_SLOW_COOLDOWN;
}

void Entity::TakeFireDamage()
{
	if( !m_isOnFire )
	{
		return;
	}

	if( m_fireCooldown > 0.f )
	{
		m_fireCooldown--;
	}
	else
	{
		m_fireCooldown = FIRE_DAMAGE_COOLDOWN;
		TakeDamage( 1 );
		m_fireTick--;
		
		if( m_fireTick == 0 )
		{
			m_isOnFire = false;
			m_fireTick = FIRE_DAMAGE_TICK;
		}
	}
}

void Entity::FireDamageVisual() const
{
	if( !m_isOnFire )
	{
		return;
	}

	float timeNow = static_cast<float>( GetCurrentTimeSeconds() );
	DrawGlow( m_position, Rgba8( 255, 0, 0 ), 0.7f, m_cosmeticRadius + cosf( timeNow * 7.f ) );
}

void Entity::BeSlowed()
{
	if( !m_isSlow )
	{
		return;
	}

	if( m_iceSlowTimer > 0.f )
	{
		m_iceSlowTimer--;
		m_velocity = m_slowedVelocity;
	}
	else
	{
		m_isSlow = false;
		m_velocity = m_originalVelocity;
		m_iceSlowTimer = ICE_SLOW_COOLDOWN;
	}
}

void Entity::IceSlowVisual() const
{
	if( !m_isSlow )
	{
		return;
	}

	float timeNow = static_cast<float>( GetCurrentTimeSeconds() );
	DrawGlow( m_position, Rgba8( 0, 255, 255 ), 0.7f, m_cosmeticRadius + cosf( timeNow * 2.f ) );
}

