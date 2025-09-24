#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Entity.hpp"
#include "Game/Debris.hpp"
#include "Game/GameCommon.hpp"

Debris::Debris( Game* owner, Vec2 const& startingPos, Vec2 const& vel, Rgba8 color )
	: Entity( owner, startingPos )
{
	m_velocity = vel;
	m_physicsRadius = DEBRIS_PHYSICS_RADIUS;
	m_cosmeticRadius = DEBRIS_COSMETIC_RADIUS;
	m_color = color;
	InitializeLocalVerts();
}

void Debris::Update( float deltaSeconds )
{
	m_position += ( m_velocity * deltaSeconds );
	m_orientationDegrees += ( m_angualrVelocity * deltaSeconds );
	m_ageSeconds += deltaSeconds;

	if( m_ageSeconds >= m_lifeTimeSeconds)
	{
		Die();
	}
}

void Debris::Render() const
{
	Vertex tempWorldVerts[ NUM_DEBRIS_VERTS ];

	for( int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; vertIndex++ )
	{
		tempWorldVerts[ vertIndex ] = m_localVerts[ vertIndex ];
		float alphaFloat = RangeMapClamped( m_ageSeconds, 0.f, m_lifeTimeSeconds, 127.f, 0.f );
		tempWorldVerts[ vertIndex ].m_color.a = static_cast<unsigned char>( alphaFloat );
	}

	TransformVertexArrayXY3D( NUM_DEBRIS_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_DEBRIS_VERTS, tempWorldVerts ); 
}

void Debris::Die() 
{
	m_isDead = true;
	m_isGarbage = true;
}

void Debris::InitializeLocalVerts()
{
	// Precompute random radii along each triangle seam
	float debrisRadii[ NUM_DEBRIS_SIDES ] = {};
	RandomNumberGenerator rng;

	for( int sideNum = 0; sideNum < NUM_DEBRIS_SIDES; sideNum++ )
	{
		debrisRadii[ sideNum ] = rng.RollRandomFloatInRange( m_physicsRadius, m_cosmeticRadius );
	}

	// Precompute 2D vertex offsets
	constexpr float degreesPerDebrisSide = 360.f / static_cast<float>( NUM_DEBRIS_SIDES );
	Vec2 debrisLocalVertPositions[ NUM_DEBRIS_SIDES ] = {};

	for( int sideNum = 0; sideNum < NUM_DEBRIS_SIDES; sideNum++ )
	{
		float degrees = degreesPerDebrisSide * static_cast<float>( sideNum );
		float radius = debrisRadii[ sideNum ];
		debrisLocalVertPositions[ sideNum ].x = radius * CosDegrees( degrees );
		debrisLocalVertPositions[ sideNum ].y = radius * SinDegrees( degrees );
	}

	// Build Triangles
	for( int triNum = 0; triNum < NUM_DEBRIS_TRIS; triNum++ )
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = ( triNum + 1 ) % NUM_DEBRIS_SIDES;
		int firstVertIndex = ( triNum * 3 );
		int secondVertIndex = ( triNum * 3 ) + 1;
		int thirdVertIndex = ( triNum * 3 ) + 2;

		Vec2 secondVertOfs = debrisLocalVertPositions[ startRadiusIndex ];
		Vec2 thirdVertOfs = debrisLocalVertPositions[ endRadiusIndex ];

		m_localVerts[ firstVertIndex ].m_pos = Vec3( 0.f, 0.f, 0.f );
		m_localVerts[ secondVertIndex ].m_pos = Vec3( secondVertOfs.x, secondVertOfs.y, 0.f );
		m_localVerts[ thirdVertIndex ].m_pos = Vec3( thirdVertOfs.x, thirdVertOfs.y, 0.f );
	}

	// Set Colors
	for( int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
}
