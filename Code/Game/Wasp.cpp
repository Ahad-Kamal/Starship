#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Wasp.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"

Wasp::Wasp( Game* owner, Vec2 const& startingPosition )
	: Entity( owner, startingPosition )
{
	m_health = 2;
	m_physicsRadius = WASP_PHYSICS_RADIUS;
	m_cosmeticRadius = WASP_COSMETIC_RADIUS;
	m_color = Rgba8( 255, 255, 0 );
	InitializeLocalVerts();
}

void Wasp::Update( float deltaSeconds )
{
	m_velocity += GetForwardNormal() * WASP_ACCELERATION * deltaSeconds;
	m_position += ( m_velocity * deltaSeconds );
}

void Wasp::Update( float deltaSeconds, PlayerShip const& ship )
{
	m_velocity += GetForwardNormal() * WASP_ACCELERATION * deltaSeconds;
	m_velocity.x = GetClamped( m_velocity.x , -50.f, 50.f );
	m_velocity.y = GetClamped( m_velocity.y, -50.f, 50.f );
	m_position += ( m_velocity * deltaSeconds );

	if( ship.IsAlive() )
	{
		m_orientationDegrees = Atan2Degrees( ship.m_position.y - m_position.y, ship.m_position.x - m_position.x );
	}
}

void Wasp::Render() const
{
	Vertex tempWorldVerts[ NUM_WASP_VERTS ];

	for( int vertIndex = 0; vertIndex < NUM_WASP_VERTS; vertIndex++ )
	{
		tempWorldVerts[ vertIndex ] = m_localVerts[ vertIndex ];
	}

	TransformVertexArrayXY3D( NUM_WASP_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_WASP_VERTS, tempWorldVerts );
}

void Wasp::InitializeLocalVerts()
{
	// Head
	m_localVerts[ 0 ].m_pos = Vec3( 2.f, 0.f, 0.f );
	m_localVerts[ 1 ].m_pos = Vec3( 1.f, 0.5f, 0.f );
	m_localVerts[ 2 ].m_pos = Vec3( 1.f, -0.5f, 0.f );

	// Body (Quad Tri 1)
	m_localVerts[ 3 ].m_pos = Vec3( -1.f, 0.5f, 0.f );
	m_localVerts[ 4 ].m_pos = Vec3( 1.f, 0.5f, 0.f );
	m_localVerts[ 5 ].m_pos = Vec3( 1.f, -0.5f, 0.f );

	// Body (Quad Tri 2)
	m_localVerts[ 6 ].m_pos = Vec3( -1.f, 0.5f, 0.f );
	m_localVerts[ 7 ].m_pos = Vec3( -1.f, -0.5f, 0.f );
	m_localVerts[ 8 ].m_pos = Vec3( 1.f, -0.5f, 0.f );

	// Left Big Wing
	m_localVerts[ 9 ].m_pos = Vec3( 0.f, 0.5f, 0.f );
	m_localVerts[ 10 ].m_pos = Vec3( 1.f, 0.5f, 0.f );
	m_localVerts[ 11 ].m_pos = Vec3( 0.5f, 2.f, 0.f );

	// Right Big Wing
	m_localVerts[ 12 ].m_pos = Vec3( 0.f, -0.5f, 0.f );
	m_localVerts[ 13 ].m_pos = Vec3( 1.f, -0.5f, 0.f );
	m_localVerts[ 14 ].m_pos = Vec3( 0.5f, -2.f, 0.f );

	// Left Small Wing
	m_localVerts[ 15 ].m_pos = Vec3( 0.f, 0.5f, 0.f );
	m_localVerts[ 16 ].m_pos = Vec3( -1.f, 0.5f, 0.f );
	m_localVerts[ 17 ].m_pos = Vec3( -0.5f, 1.f, 0.f );

	// Right Small Wing
	m_localVerts[ 18 ].m_pos = Vec3( 0.f, -0.5f, 0.f );
	m_localVerts[ 19 ].m_pos = Vec3( -1.f, -0.5f, 0.f );
	m_localVerts[ 20 ].m_pos = Vec3( -0.5f, -1.f, 0.f );

	// Tail (Quad Tri 2)
	m_localVerts[ 21 ].m_pos = Vec3( -1.f, 0.5f, 0.f );
	m_localVerts[ 22 ].m_pos = Vec3( -1.f, -0.5f, 0.f );
	m_localVerts[ 23 ].m_pos = Vec3( -2.f, 0.f, 0.f );

	for( int vertIndex = 0; vertIndex < NUM_WASP_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
}

