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
	
	if( m_isThrusting )
	{
		m_velocity += this->GetForwardNormal() * m_thrustFraction * PLAYER_SHIP_ACCELERATION * deltaSeconds;
	}
	m_velocity.GetClamped( 30.f );
	m_position += m_velocity * deltaSeconds;
}

void PlayerShip::Render() const
{
	Vertex tempShipWorldVerts[ NUM_SHIP_VERTS_TOTAL ];
	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS_TOTAL; vertIndex++ )
	{
		tempShipWorldVerts[ vertIndex ] = m_localVerts[ vertIndex ];
	}

	TransformVertexArrayXY3D( NUM_SHIP_VERTS_TOTAL, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS_TOTAL, tempShipWorldVerts );
}

void PlayerShip::Die()
{
	m_isDead = true;
	m_lives--;

	int count = g_rng->RollRandomIntInRange( 5, 30 );
	m_game->SpawnNewDebrisCluster( count, m_position, m_velocity * 0.5f, Vec2( 1.f, 1.f), m_color, 1.f);
	m_game->m_isShaking = true;
}

void PlayerShip::InitializeLocalVerts()
{
	// Nose Cone
	m_localVerts[ 0 ].m_pos = Vec3( 1.f, 0.f, 0.f );
	m_localVerts[ 1 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[ 2 ].m_pos = Vec3( 0.f, -1.f, 0.f );

	// Left Wing
	m_localVerts[ 3 ].m_pos = Vec3( 2.f, 1.f, 0.f );
	m_localVerts[ 4 ].m_pos = Vec3( 0.f, 2.f, 0.f );
	m_localVerts[ 5 ].m_pos = Vec3( -2.f, 1.f, 0.f );

	// Right Wing
	m_localVerts[ 6 ].m_pos = Vec3( 2.f, -1.f, 0.f );
	m_localVerts[ 7 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	m_localVerts[ 8 ].m_pos = Vec3( 0.f, -2.f, 0.f );

	// Body (Quad Tri 1)
	m_localVerts[ 9 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[ 10 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	m_localVerts[ 11 ].m_pos = Vec3( 0.f, -1.f, 0.f );

	// Body (Quad Tri 2)
	m_localVerts[ 12 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[ 13 ].m_pos = Vec3( -2.f, 1.f, 0.f );
	m_localVerts[ 14 ].m_pos = Vec3( -2.f, -1.f, 0.f );

	// Thruster
	m_localVerts[ 15 ].m_pos = Vec3( -2.f, 1.f, 0.f );
	m_localVerts[ 16 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	m_localVerts[ 17 ].m_pos = Vec3( -5.f, 0.f, 0.f );

	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
	/*for( int vertIndex = NUM_SHIP_VERTS; vertIndex < NUM_SHIP_VERTS + NUM_THRUST_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = Rgba8( 200, 0, 0, 200 );
	}*/
}

void PlayerShip::UpdateFromKeyboard( float deltaSeconds )
{
	
	if( g_engine->m_input->wasKeyJustPressed( ' ' ) && IsAlive() )
	{
		Vec2 bulletOffset = this->GetForwardNormal();
		m_game->SpawnBullet( m_position + bulletOffset, m_orientationDegrees );
		SoundID testSound = g_engine->m_audio->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
		g_engine->m_audio->StartSound( testSound );
	}

	if( g_engine->m_input->wasKeyJustPressed( 'I' ) )
	{
		m_game->SpawnRandomAsteroid();
	}

	if( g_engine->m_input->isKeyDown( 'E' ) && IsAlive() )
	{
		m_velocity += this->GetForwardNormal() * PLAYER_SHIP_ACCELERATION * deltaSeconds;
		ActivateThrust();
	}
	else
	{
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
	if( leftStickMagnitude > 0.f )
	{
		m_isThrusting = true;
		m_thrustFraction = leftStickMagnitude;
		m_orientationDegrees = controller.GetLeftStick().GetOrientationDegrees();
		ActivateThrust();
	}
	else
	{
		m_isThrusting = false;
		DeactivateThrust();
	}

	if( controller.WasButtonJustPressed( XboxButtonID::A ) )
	{
		Vec2 bulletOffset = this->GetForwardNormal();
		m_game->SpawnBullet( m_position + bulletOffset, m_orientationDegrees );
	}
}

void PlayerShip::ActivateThrust()
{
	float randomAlpha = g_rng->RollRandomFloatInRange( 0, 200 );
	for( int vertIndex = NUM_SHIP_VERTS; vertIndex < NUM_SHIP_VERTS + NUM_THRUST_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = Rgba8( 200, 0, 0, randomAlpha );
	}
}

void PlayerShip::DeactivateThrust()
{
	for( int vertIndex = NUM_SHIP_VERTS; vertIndex < NUM_SHIP_VERTS + NUM_THRUST_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = Rgba8( 200, 0, 0, 0 );
	}
}

void PlayerShip::BounceOffWalls()
{
	if( m_position.x > WORLD_SIZE_X - m_cosmeticRadius )
	{
		m_velocity.x *= -1;
	}
	else if( m_position.y > WORLD_SIZE_Y - m_cosmeticRadius )
	{
		m_velocity.y *= -1;
	}
	else if( m_position.x < m_cosmeticRadius )
	{
		m_velocity.x *= -1;
	}
	else if( m_position. y < m_cosmeticRadius )
	{
		m_velocity.y *= -1;
	}
}

void PlayerShip::Respawn()
{
	m_position = Vec2( WORLD_CENTER_X, WORLD_CENTER_Y );
	m_velocity = Vec2( 0.f, 0.f );
	m_orientationDegrees = 0.f;
	m_isDead = false;
}

void createFakePlayerShip( Vertex verts[], float transparency )
{
	// Nose Cone
	verts[ 0 ].m_pos = Vec3( 1.f, 0.f, 0.f );
	verts[ 1 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	verts[ 2 ].m_pos = Vec3( 0.f, -1.f, 0.f );

	// Left Wing
	verts[ 3 ].m_pos = Vec3( 2.f, 1.f, 0.f );
	verts[ 4 ].m_pos = Vec3( 0.f, 2.f, 0.f );
	verts[ 5 ].m_pos = Vec3( -2.f, 1.f, 0.f );

	// Right Wing
	verts[ 6 ].m_pos = Vec3( 2.f, -1.f, 0.f );
	verts[ 7 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	verts[ 8 ].m_pos = Vec3( 0.f, -2.f, 0.f );

	// Body (Quad Tri 1)
	verts[ 9 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	verts[ 10 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	verts[ 11 ].m_pos = Vec3( 0.f, -1.f, 0.f );

	// Body (Quad Tri 2)
	verts[ 12 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	verts[ 13 ].m_pos = Vec3( -2.f, 1.f, 0.f );
	verts[ 14 ].m_pos = Vec3( -2.f, -1.f, 0.f );

	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; vertIndex++ )
	{
		verts[ vertIndex ].m_color = Rgba8( 102, 153, 204, static_cast<unsigned int>( transparency ) );
	}
}
