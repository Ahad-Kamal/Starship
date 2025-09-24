#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"


Bullet::Bullet( Game* owner, Vec2 const& startPos )
	: Entity( owner, startPos )
{
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	InitializeLocalVerts();
}

Bullet::~Bullet()
{

}

void Bullet::Update( float deltaSeconds )
{
	m_position += (m_velocity * deltaSeconds );

	if( IsOffScreen() )
	{
		m_isDead = true;
		m_isGarbage = true;
	}
}

void Bullet::Render() const
{
	Vertex tempWorldVerts[NUM_BULLET_VERTS];

	for( int vertIndex = 0; vertIndex < NUM_BULLET_VERTS; vertIndex++ )
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D( NUM_BULLET_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_BULLET_VERTS, tempWorldVerts );
}

void Bullet::Die()
{
	m_isDead = true;
	m_isGarbage = true;

	m_game->SpawnNewDebrisCluster( 2, m_position, m_velocity * 0.5f, -3.f * GetForwardNormal(), Rgba8( 250.f, 250.f, 250.f ), 0.25f );
}

void Bullet::InitializeLocalVerts()
{
	// Bullet
	m_localVerts[0].m_pos = Vec3( 0.5f, 0.f, 0.f );
	m_localVerts[1].m_pos = Vec3( 0.f, 0.5f, 0.f );
	m_localVerts[2].m_pos = Vec3( 0.f, -0.5f, 0.f );
	m_localVerts[0].m_color = Rgba8( 255, 255, 255, 255 );
	m_localVerts[1].m_color = Rgba8( 255, 255, 0, 255 );
	m_localVerts[2].m_color = Rgba8( 255, 255, 0, 255 );

	// Trail
	m_localVerts[3].m_pos = Vec3( 0.f, -0.5f, 0.f );
	m_localVerts[4].m_pos = Vec3( 0.f, 0.5f, 0.f );
	m_localVerts[5].m_pos = Vec3( -2.f, 0.f, 0.f );
	m_localVerts[3].m_color = Rgba8( 255, 0, 0, 255 );
	m_localVerts[4].m_color = Rgba8( 255, 0, 0, 255 );
	m_localVerts[5].m_color = Rgba8( 255, 0, 0, 0 );
}

