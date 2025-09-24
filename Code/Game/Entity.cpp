#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
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

	m_game->SpawnNewDebrisCluster( 5, m_position, m_velocity * 0.5f, GetForwardNormal(), m_color, 1.f );
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
	if( m_position.x > WORLD_SIZE_X + m_cosmeticRadius )
	{
		return true;
	}
	else if( m_position.y > WORLD_SIZE_Y + m_cosmeticRadius )
	{
		return true;
	}
	else if( m_position.x < -m_cosmeticRadius )
	{
		return true;
	}
	else if( m_position. y < -m_cosmeticRadius )
	{
		return true;
	}

	return false;
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) );
}

