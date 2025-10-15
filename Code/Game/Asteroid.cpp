#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Game/Asteroid.hpp"
#include "Game/GameCommon.hpp"

Asteroid::Asteroid( Game* owner, Vec2 const& startPos )
	: Entity( owner, startPos )
{
	m_health = 3;
	m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
	m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;
	m_color = Rgba8( 100, 100, 100 );
	InitializeLocalVerts();
}

Asteroid::~Asteroid()
{

}

void Asteroid::Update( float deltaSeconds )
{
	m_position += ( m_velocity * deltaSeconds );
	m_orientationDegrees += ( m_angualrVelocity * deltaSeconds );

	if( IsOffScreen() )
	{
		WrapAroundScreen();
	}
}

void Asteroid::Render() const
{
	Vertex tempWorldVerts[ NUM_ASTEROID_VERTS ];

	for( int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; vertIndex++ )
	{
		tempWorldVerts[ vertIndex ] = m_localVerts[ vertIndex ];
	}

	DrawGlow( m_position, m_color, 0.5f, ASTEROID_COSMETIC_RADIUS + 5.f );
	TransformVertexArrayXY3D( NUM_ASTEROID_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_ASTEROID_VERTS, tempWorldVerts );
}

void Asteroid::Die()
{
	m_isDead = true;
	m_isGarbage = true;

	int count = g_rng->RollRandomIntInRange( 30, 50 );
	m_game->SpawnNewDebrisCluster( count, m_position, m_velocity * 0.5f, GetForwardNormal(), m_color, 1.f );

	if( m_isFieryAsteroid )
	{
		m_game->SpawnNewExplosion( m_position, m_color, true );
	}
	else if( m_isIcyAsteroid )
	{
		m_game->SpawnNewExplosion( m_position, m_color, false );
	}

	m_game->AddCameraShake( 2.f );
}

void Asteroid::InitializeLocalVerts()
{
	// Precompute random radii along each triangle seam
	float asteroidRadii[ NUM_ASTEROID_SIDES ] = {};
	RandomNumberGenerator rng;

	for( int sideNum = 0; sideNum < NUM_ASTEROID_SIDES; sideNum++ )
	{
		asteroidRadii[ sideNum ] = rng.RollRandomFloatInRange( m_physicsRadius, m_cosmeticRadius );
	}

	// Precompute 2D vertex offsets
	constexpr float degreesPerAsteroidSide = 360.f / static_cast<float>( NUM_ASTEROID_SIDES );
	Vec2 asteroidLocalVertPositions[ NUM_ASTEROID_SIDES ] = {};

	for( int sideNum = 0; sideNum < NUM_ASTEROID_SIDES; sideNum++ )
	{
		float degrees = degreesPerAsteroidSide * static_cast<float>( sideNum );
		float radius = asteroidRadii[ sideNum ];
		asteroidLocalVertPositions[ sideNum ].x = radius * CosDegrees( degrees );
		asteroidLocalVertPositions[ sideNum ].y = radius * SinDegrees( degrees );
	}

	// Build Triangles
	for( int triNum = 0; triNum < NUM_ASTEROID_TRIS; triNum++ )
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = ( triNum + 1 ) % NUM_ASTEROID_SIDES;
		int firstVertIndex = ( triNum * 3 );
		int secondVertIndex = ( triNum * 3 ) + 1;
		int thirdVertIndex = ( triNum * 3 ) + 2;

		Vec2 secondVertOfs = asteroidLocalVertPositions[ startRadiusIndex ];
		Vec2 thirdVertOfs = asteroidLocalVertPositions[ endRadiusIndex ];

		m_localVerts[ firstVertIndex ].m_pos = Vec3( 0.f, 0.f, 0.f );
		m_localVerts[ secondVertIndex ].m_pos = Vec3( secondVertOfs.x, secondVertOfs.y, 0.f );
		m_localVerts[ thirdVertIndex ].m_pos = Vec3( thirdVertOfs.x, thirdVertOfs.y, 0.f );
	}
}

