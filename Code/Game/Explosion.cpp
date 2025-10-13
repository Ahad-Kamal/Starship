#include "Game/Explosion.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

Explosion::Explosion( Game* owner, Vec2 const& startPos, Rgba8 color, bool explosionType ) // true if fiery, false if icy
	: Entity( owner, startPos )
{
	m_physicsRadius = EXPLOSION_PHYSICS_RADIUS;
	m_cosmeticRadius = EXPLOSION_COSMETIC_RADIUS;
	m_color = color;
	
	if( explosionType )
	{
		m_isFieryExplosion = true;
	}
	else
	{
		m_isIcyExplosion = true;
	}

	InitializeLocalVerts();
}

void Explosion::Update( float deltaSeconds )
{
	m_ageSeconds += deltaSeconds;
	if( m_ageSeconds >= m_lifeTimeSeconds )
	{
		Die();
	}
}

void Explosion::Render() const
{
	Vertex tempWorldVerts[ NUM_EXPLOSION_VERTS ];

	for( int vertIndex = 0; vertIndex < NUM_EXPLOSION_VERTS; vertIndex++ )
	{
		tempWorldVerts[ vertIndex ] = m_localVerts[ vertIndex ];
		float alphaFloat = RangeMapClamped( m_ageSeconds, 0.f, m_lifeTimeSeconds, 255.f, 0.f );
		tempWorldVerts[ vertIndex ].m_color.a = static_cast<unsigned char>( alphaFloat );
	}

	TransformVertexArrayXY3D( NUM_EXPLOSION_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_EXPLOSION_VERTS, tempWorldVerts );
}

void Explosion::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}

void Explosion::InitializeLocalVerts()
{
	// Precompute radii along each triangle seam
	float explosionRadii[ NUM_EXPLOSION_SIDES ] = {};
	RandomNumberGenerator rng;

	for( int sideNum = 0; sideNum < NUM_EXPLOSION_SIDES; sideNum++ )
	{
		explosionRadii[ sideNum ] = m_cosmeticRadius;
	}

	// Precompute 2D vertex offsets
	constexpr float degreesPerExplosionSide = 360.f / static_cast<float>( NUM_EXPLOSION_SIDES );
	Vec2 explosionLocalVertPositions[ NUM_EXPLOSION_SIDES ] = {};

	for( int sideNum = 0; sideNum < NUM_EXPLOSION_SIDES; sideNum++ )
	{
		float degrees = degreesPerExplosionSide * static_cast<float>( sideNum );
		float radius = explosionRadii[ sideNum ];
		explosionLocalVertPositions[ sideNum ].x = radius * CosDegrees( degrees );
		explosionLocalVertPositions[ sideNum ].y = radius * SinDegrees( degrees );
	}

	// Build Triangles
	for( int triNum = 0; triNum < NUM_EXPLOSION_TRIS; triNum++ )
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = ( triNum + 1 ) % NUM_EXPLOSION_SIDES;
		int firstVertIndex = ( triNum * 3 );
		int secondVertIndex = ( triNum * 3 ) + 1;
		int thirdVertIndex = ( triNum * 3 ) + 2;

		Vec2 secondVertOfs = explosionLocalVertPositions[ startRadiusIndex ];
		Vec2 thirdVertOfs = explosionLocalVertPositions[ endRadiusIndex ];

		m_localVerts[ firstVertIndex ].m_pos = Vec3( 0.f, 0.f, 0.f );
		m_localVerts[ secondVertIndex ].m_pos = Vec3( secondVertOfs.x, secondVertOfs.y, 0.f );
		m_localVerts[ thirdVertIndex ].m_pos = Vec3( thirdVertOfs.x, thirdVertOfs.y, 0.f );

		m_localVerts[ firstVertIndex ].m_color = Rgba8( m_color.r, m_color.g, m_color.b );
		m_localVerts[ secondVertIndex ].m_color = Rgba8( m_color.r, m_color.g, m_color.b, 127 );
		m_localVerts[ thirdVertIndex ].m_color = Rgba8( m_color.r, m_color.g, m_color.b, 127 );
	}

	// Set Colors
	/*for( int vertIndex = 0; vertIndex < NUM_EXPLOSION_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}*/
}

