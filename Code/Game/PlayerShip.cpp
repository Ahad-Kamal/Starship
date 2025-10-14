#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"

PlayerShip::PlayerShip(Game* owner, Vec2 const& startingPosition)
	: Entity( owner, startingPosition )
{
	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	m_color = Rgba8( 102, 153, 204);
	InitializeLocalVerts();
}

PlayerShip::PlayerShip(Game* owner, Vec2 const& startingPosition, Vec2 const& startingVelocity)
	: Entity( owner, startingPosition )
{
	m_velocity = startingVelocity;
	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	InitializeLocalVerts();
}

void PlayerShip::Update(float deltaSeconds)
{
	UpdateFromKeyboard( deltaSeconds );	
	UpdateFromController();
	BounceOffWalls();
	
	if( m_isControllerThrusting )
	{
		m_velocity += this->GetForwardNormal() * m_thrustFraction * PLAYER_SHIP_ACCELERATION * deltaSeconds;
	}
	m_velocity.GetClamped( 30.f );
	m_position += m_velocity * deltaSeconds;

	if( m_fireBulletCooldown < MAX_FIRE_BULLET_COOLDOWN )
	{
 		m_fireBulletCooldown += deltaSeconds;
		m_fireBulletCooldown = GetClamped( m_fireBulletCooldown, 0.f, MAX_FIRE_BULLET_COOLDOWN );
	}
	if( m_iceBulletCooldown < MAX_ICE_BULLET_COOLDOWN )
	{
		m_iceBulletCooldown += deltaSeconds;
		m_iceBulletCooldown = GetClamped( m_iceBulletCooldown, 0.f, MAX_ICE_BULLET_COOLDOWN );
	}

	CountdownInvincibility();
}
void PlayerShip::Render() const
{
	Vertex tempShipWorldVerts[ NUM_SHIP_VERTS_TOTAL ];
	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS_TOTAL; vertIndex++ )
	{
		tempShipWorldVerts[ vertIndex ] = m_localVerts[ vertIndex ];
	}

	/*float timeNow = GetCurrentTimeSeconds();
	DrawGlow( m_position, Rgba8( 255, 255, 255), 0.5f, 5.f + sinf( timeNow * 5.f ) );*/
	TransformVertexArrayXY3D( NUM_SHIP_VERTS_TOTAL, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS_TOTAL, tempShipWorldVerts );
}

void PlayerShip::Die()
{
	m_isDead = true;
	m_lives--;
	m_velocity = Vec2( 0.f, 0.f );

	int count = g_rng->RollRandomIntInRange( 5, 30 );
	m_game->SpawnNewDebrisCluster( count, m_position, m_velocity * 0.5f, Vec2( 1.f, 1.f), Rgba8( 255, 255, 255 ), 1.f);
	m_game->SpawnNewDebrisCluster( count, m_position, m_velocity * 0.5f, Vec2( 1.f, 1.f), Rgba8( 51, 204, 255 ), 1.f);
	m_game->SpawnNewDebrisCluster( count, m_position, m_velocity * 0.5f, Vec2( 1.f, 1.f), Rgba8( 255, 25, 25 ), 1.f);
	m_game->AddCameraShake( 5.f );
}

void PlayerShip::InitializeLocalVerts()
{
	// Nose Cone
	m_localVerts[ 0 ].m_pos = Vec3( 1.f, 0.f, 0.f );
	m_localVerts[ 1 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[ 2 ].m_pos = Vec3( 0.f, -1.f, 0.f );

	m_localVerts[ 0 ].m_color = Rgba8( 255, 255, 255 );
	m_localVerts[ 1 ].m_color = Rgba8( 255, 255, 255 );
	m_localVerts[ 2 ].m_color = Rgba8( 255, 255, 255 );

	// Left Wing
	m_localVerts[ 3 ].m_pos = Vec3( 2.f, 1.f, 0.f );
	m_localVerts[ 4 ].m_pos = Vec3( 0.f, 2.f, 0.f );
	m_localVerts[ 5 ].m_pos = Vec3( -2.f, 1.f, 0.f );

	m_localVerts[ 3 ].m_color = Rgba8( 51, 204, 255 );
	m_localVerts[ 4 ].m_color = Rgba8( 51, 204, 255 );
	m_localVerts[ 5 ].m_color = Rgba8( 51, 204, 255 );

	// Right Wing
	m_localVerts[ 6 ].m_pos = Vec3( 2.f, -1.f, 0.f );
	m_localVerts[ 7 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	m_localVerts[ 8 ].m_pos = Vec3( 0.f, -2.f, 0.f );

	m_localVerts[ 6 ].m_color = Rgba8( 255, 25, 25 );
	m_localVerts[ 7 ].m_color = Rgba8( 255, 25, 25 );
	m_localVerts[ 8 ].m_color = Rgba8( 255, 25, 25 );

	// Body (Quad Tri 1)
	m_localVerts[ 9 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[ 10 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	m_localVerts[ 11 ].m_pos = Vec3( 0.f, -1.f, 0.f );

	m_localVerts[ 9 ].m_color = Rgba8( 255, 255, 255 );
	m_localVerts[ 10 ].m_color = Rgba8( 255, 255, 255 );
	m_localVerts[ 11 ].m_color = Rgba8( 255, 255, 255 );

	// Body (Quad Tri 2)
	m_localVerts[ 12 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[ 13 ].m_pos = Vec3( -2.f, 1.f, 0.f );
	m_localVerts[ 14 ].m_pos = Vec3( -2.f, -1.f, 0.f );

	m_localVerts[ 12 ].m_color = Rgba8( 255, 255, 255 );
	m_localVerts[ 13 ].m_color = Rgba8( 255, 255, 255 );
	m_localVerts[ 11 ].m_color = Rgba8( 255, 255, 255 );

	// Thruster
	m_localVerts[ 15 ].m_pos = Vec3( -2.f, 1.f, 0.f );
	m_localVerts[ 16 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	m_localVerts[ 17 ].m_pos = Vec3( -4.f, 0.f, 0.f );
}

void PlayerShip::UpdateFromKeyboard( float deltaSeconds )
{
	if( g_engine->m_input->isKeyDown( 'W' ) && IsAlive() && m_fireBulletCooldown == MAX_FIRE_BULLET_COOLDOWN )
	{
		m_fireBulletCooldown = 0.f;
		Vec2 bulletOffset = Vec2( 2.f, -1.f ).GetRotatedByDegrees( m_orientationDegrees );
		m_game->SpawnFireBullet( m_position + bulletOffset, m_orientationDegrees );
	}
	if( g_engine->m_input->isKeyDown( 'R' ) && IsAlive() && m_iceBulletCooldown == MAX_ICE_BULLET_COOLDOWN )
	{
		m_fireBulletCooldown = 0.f;
		Vec2 bulletOffset = Vec2( 2.f, 1.f ).GetRotatedByDegrees( m_orientationDegrees );
		m_game->SpawnIceBullet( m_position + bulletOffset, m_orientationDegrees );
	}

	if( g_engine->m_input->wasKeyJustPressed( 'I' ) )
	{
		m_game->SpawnRandomFieryAsteroid();
	}

	if( g_engine->m_input->isKeyDown( 'E' ) && IsAlive() )
	{
		m_velocity += this->GetForwardNormal() * PLAYER_SHIP_ACCELERATION * deltaSeconds;
		m_isKeyboardThrusting = true;
		ActivateThrust();
	}
	else if( !m_isControllerThrusting )
	{
		m_isKeyboardThrusting = false;
		DeactivateThrust();
	}

	if( g_engine->m_input->isKeyDown( 'S' ) && IsAlive() )
	{
		m_orientationDegrees += PLAYER_SHIP_TURN_SPEED * deltaSeconds;
	}

	if( g_engine->m_input->isKeyDown( 'F' ) && IsAlive() )
	{
		m_orientationDegrees -= PLAYER_SHIP_TURN_SPEED * deltaSeconds;
	}

	if( g_engine->m_input->isKeyDown( 'N' ) && !IsAlive() && m_lives > 0 )
	{
		Respawn();
	}
	
}

void PlayerShip::UpdateFromController()
{
	XboxController const& controller = g_engine->m_input->m_controllers[ 0 ];

	if( m_isDead )
	{
		if( controller.WasButtonJustPressed( XboxButtonID::START ) && m_lives > 0 )
		{
			Respawn();
		}
	}

	float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
	float rightStickMagnitude = controller.GetRightStick().GetMagnitude();

	if( leftStickMagnitude > 0.f && IsAlive() )
	{
		m_isControllerThrusting = true;
		m_thrustFraction = leftStickMagnitude;
		m_orientationDegrees = controller.GetLeftStick().GetOrientationDegrees();
		ActivateThrust();
	}
	else if( !m_isKeyboardThrusting )
	{
		m_isControllerThrusting = false;
		DeactivateThrust();
	}

	if( rightStickMagnitude > 0.f && !m_isControllerThrusting && IsAlive() )
	{
		m_orientationDegrees = controller.GetRightStick().GetOrientationDegrees();
	}

	if( controller.GetRightTrigger() == 1.f && IsAlive() && m_fireBulletCooldown == MAX_FIRE_BULLET_COOLDOWN )
	{
		m_fireBulletCooldown = 0.f;
		Vec2 bulletOffset = Vec2( 2.f, -1.f ).GetRotatedByDegrees( m_orientationDegrees );
		m_game->SpawnFireBullet( m_position + bulletOffset, m_orientationDegrees );
	}
	if( controller.GetLeftTrigger() == 1.f && IsAlive() && m_iceBulletCooldown == MAX_ICE_BULLET_COOLDOWN )
	{
		m_iceBulletCooldown = 0.f;
		Vec2 bulletOffset = Vec2( 2.f, 1.f ).GetRotatedByDegrees( m_orientationDegrees );
		m_game->SpawnIceBullet( m_position + bulletOffset, m_orientationDegrees );
	}
}

void PlayerShip::ActivateThrust()
{
	float randomAlpha = g_rng->RollRandomFloatInRange( 80.f, 240.f );
	float randomLength = GetClamped( g_rng->RollRandomFloatInRange( -4.f, -8.f ) * m_thrustFraction , -8.f, -2.f );

	for( int vertIndex = NUM_SHIP_VERTS; vertIndex < NUM_SHIP_VERTS + NUM_THRUST_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = Rgba8( 206, 153, 255, static_cast<unsigned int>( randomAlpha ) );
	}
	m_localVerts[ NUM_SHIP_VERTS_TOTAL - 1 ].m_pos.x = randomLength;
}

void PlayerShip::DeactivateThrust()
{
	for( int vertIndex = NUM_SHIP_VERTS; vertIndex < NUM_SHIP_VERTS + NUM_THRUST_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = Rgba8( 206, 153, 255, 0 );
	}
	m_localVerts[ NUM_SHIP_VERTS_TOTAL - 1 ].m_pos.x = -4.f;
}

void PlayerShip::Respawn()
{
	//m_position = Vec2( WORLD_CENTER_X, WORLD_CENTER_Y );
	m_velocity = Vec2( 0.f, 0.f );
	m_orientationDegrees = 0.f;
	m_isDead = false;

	m_isInvincible = true;
	for( int shipVerts = 0; shipVerts < NUM_SHIP_VERTS; shipVerts++ )
	{
		m_localVerts[ shipVerts ].m_color.a = 128;
	}
}

void PlayerShip::CountdownInvincibility()
{
	if( !m_isInvincible )
	{
		return;
	}

	if( m_invincibilityTime <= 0 )
	{
		m_isInvincible = false;
		for( int shipVerts = 0; shipVerts < NUM_SHIP_VERTS; shipVerts++ )
		{
			m_localVerts[ shipVerts ].m_color.a = 255;
		}
		m_invincibilityTime = INVINCIBILITY_DURATION;
	}
	else
	{
		m_invincibilityTime--;
	}
}

void createFakePlayerShip( Vertex verts[], [[maybe_unused]] float transparency )
{
	// Nose Cone
	verts[ 0 ].m_pos = Vec3( 1.f, 0.f, 0.f );
	verts[ 1 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	verts[ 2 ].m_pos = Vec3( 0.f, -1.f, 0.f );

	verts[ 0 ].m_color = Rgba8( 255, 255, 255 );
	verts[ 1 ].m_color = Rgba8( 255, 255, 255 );
	verts[ 2 ].m_color = Rgba8( 255, 255, 255 );

	// Left Wing
	verts[ 3 ].m_pos = Vec3( 2.f, 1.f, 0.f );
	verts[ 4 ].m_pos = Vec3( 0.f, 2.f, 0.f );
	verts[ 5 ].m_pos = Vec3( -2.f, 1.f, 0.f );

	verts[ 3 ].m_color = Rgba8( 51, 204, 255 );
	verts[ 4 ].m_color = Rgba8( 51, 204, 255 );
	verts[ 5 ].m_color = Rgba8( 51, 204, 255 );

	// Right Wing
	verts[ 6 ].m_pos = Vec3( 2.f, -1.f, 0.f );
	verts[ 7 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	verts[ 8 ].m_pos = Vec3( 0.f, -2.f, 0.f );

	verts[ 6 ].m_color = Rgba8( 255, 25, 25 );
	verts[ 7 ].m_color = Rgba8( 255, 25, 25 );
	verts[ 8 ].m_color = Rgba8( 255, 25, 25 );

	// Body (Quad Tri 1)
	verts[ 9 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	verts[ 10 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	verts[ 11 ].m_pos = Vec3( 0.f, -1.f, 0.f );

	verts[ 9 ].m_color = Rgba8( 255, 255, 255 );
	verts[ 10 ].m_color = Rgba8( 255, 255, 255 );
	verts[ 11 ].m_color = Rgba8( 255, 255, 255 );

	// Body (Quad Tri 2)
	verts[ 12 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	verts[ 13 ].m_pos = Vec3( -2.f, 1.f, 0.f );
	verts[ 14 ].m_pos = Vec3( -2.f, -1.f, 0.f );

	verts[ 12 ].m_color = Rgba8( 255, 255, 255 );
	verts[ 13 ].m_color = Rgba8( 255, 255, 255 );
	verts[ 11 ].m_color = Rgba8( 255, 255, 255 );
}
