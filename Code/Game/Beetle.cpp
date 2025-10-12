#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Beetle.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"

Beetle::Beetle( Game* owner, Vec2 const& startingPosition )
	: Entity( owner, startingPosition )
{
	m_health = 10;
	m_physicsRadius = BEETLE_PHYSICS_RADIUS;
	m_cosmeticRadius = BEETLE_COSMETIC_RADIUS;
	m_color = Rgba8( 0, 255, 0 );
	InitializeLocalVerts();
}

void Beetle::Update( float deltaSeconds )
{
	m_velocity += GetForwardNormal() * deltaSeconds;
	m_position += ( m_velocity * deltaSeconds );
}

void Beetle::Update( float deltaSeconds, PlayerShip const& ship )
{
	m_position += ( m_velocity * GetForwardNormal() * deltaSeconds );

	if( ship.IsAlive() )
	{
		m_orientationDegrees = Atan2Degrees( ship.m_position.y - m_position.y, ship.m_position.x - m_position.x );
	}
}

void Beetle::Render() const
{
	Vertex tempWorldVerts[ NUM_BEETLE_VERTS ];

	for( int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; vertIndex++ )
	{
		tempWorldVerts[ vertIndex ] = m_localVerts[ vertIndex ];
	}

	TransformVertexArrayXY3D( NUM_BEETLE_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_BEETLE_VERTS, tempWorldVerts );
}

void Beetle::InitializeLocalVerts()
{
	// Left Body
	m_localVerts[ 0 ].m_pos = Vec3( 0.f, 2.f, 0.f );
	m_localVerts[ 1 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[ 2 ].m_pos = Vec3( 2.f, 1.f, 0.f );

	// Body (Quad Tri 1)
	m_localVerts[ 3 ].m_pos = Vec3( 1.f, 0.f, 0.f );
	m_localVerts[ 4 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[ 5 ].m_pos = Vec3( 2.f, 1.f, 0.f );

	// Body (Quad Tri 2)
	m_localVerts[ 6 ].m_pos = Vec3( 1.f, 0.f, 0.f );
	m_localVerts[ 7 ].m_pos = Vec3( 2.f, -1.f, 0.f );
	m_localVerts[ 8 ].m_pos = Vec3( 0.f, -1.f, 0.f );

	// Right Body
	m_localVerts[ 9 ].m_pos = Vec3( 0.f, -2.f, 0.f );
	m_localVerts[ 10 ].m_pos = Vec3( 2.f, -1.f, 0.f );
	m_localVerts[ 11 ].m_pos = Vec3( 0.f, -1.f, 0.f );

	// Back (Quad Tri 1)
	m_localVerts[ 12 ].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[ 13 ].m_pos = Vec3( 0.f, -1.f, 0.f );
	m_localVerts[ 14 ].m_pos = Vec3( -1.f, 0.f, 0.f );

	// Back (Quad Tri 2)
	m_localVerts[ 15 ].m_pos = Vec3( -2.f, 1.f, 0.f );
	m_localVerts[ 16 ].m_pos = Vec3( -2.f, -1.f, 0.f );
	m_localVerts[ 17 ].m_pos = Vec3( -1.f, 0.f, 0.f );

	for( int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
}

