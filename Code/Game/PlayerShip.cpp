#include "PlayerShip.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

PlayerShip::PlayerShip(Game* owner, Vec2 const& startingPosition)
	: Entity( owner, startingPosition )
{
	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	InitializeLocalVerts();
}

PlayerShip::PlayerShip(Game* owner, Vec2 const& startingPosition, Vec2 const& startingVelocity)
	: Entity( owner, startingPosition )
{
	m_velocity = startingVelocity;
}

void PlayerShip::Update(float deltaSeconds)
{
	UpdateFromKeyboard( deltaSeconds );
	BounceOffWalls();
	
	m_position += m_velocity * deltaSeconds;
}

void PlayerShip::Render() const
{
	if( m_isDead )
	{
		return;
	}

	Vertex tempShipWorldVerts[NUM_SHIP_VERTS];
	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; vertIndex++ )
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D( NUM_SHIP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, tempShipWorldVerts );

	// Assignment 1 PlayerShip
	/*Vertex shipVerts[3];

	shipVerts[0].m_pos = Vec3( (m_position.x ) + 4.f, m_position.y, 0.f);
	shipVerts[1].m_pos = Vec3( (m_position.x ) - 2.f, m_position.y + 2.0f, 0.f);
	shipVerts[2].m_pos = Vec3( (m_position.x ) - 2.f, m_position.y - 2.0f, 0.f);

	shipVerts[0].m_color = Rgba8( 255, 255, 255, 255 );
	shipVerts[1].m_color = Rgba8( 0, 0, 0, 255 );
	shipVerts[2].m_color = Rgba8( 0, 127, 255, 255 );

	shipVerts[0].m_uvTexCoords = Vec2( 0.f, 0.f );
	shipVerts[1].m_uvTexCoords = Vec2( 0.f, 0.f );
	shipVerts[2].m_uvTexCoords = Vec2( 0.f, 0.f );
	
	g_engine->m_render->DrawVertexArray( 3, shipVerts );*/
}

void PlayerShip::InitializeLocalVerts()
{
	// Nose Cone
	m_localVerts[0].m_pos = Vec3( 1.f, 0.f, 0.f );
	m_localVerts[1].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[2].m_pos = Vec3( 0.f, -1.f, 0.f );

	// Left Wing
	m_localVerts[3].m_pos = Vec3( 2.f, 1.f, 0.f );
	m_localVerts[4].m_pos = Vec3( 0.f, 2.f, 0.f );
	m_localVerts[5].m_pos = Vec3( -2.f, 1.f, 0.f );

	// Right Wing
	m_localVerts[6].m_pos = Vec3( 2.f, -1.f, 0.f );
	m_localVerts[7].m_pos = Vec3( -2.f, -1.f, 0.f );
	m_localVerts[8].m_pos = Vec3( 0.f, -2.f, 0.f );

	// Body (Quad Tri 1)
	m_localVerts[9].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[10].m_pos = Vec3( -2.f, -1.f, 0.f );
	m_localVerts[11].m_pos = Vec3( 0.f, -1.f, 0.f );

	// Body (Quad Tri 2)
	m_localVerts[12].m_pos = Vec3( 0.f, 1.f, 0.f );
	m_localVerts[13].m_pos = Vec3( -2.f, 1.f, 0.f );
	m_localVerts[14].m_pos = Vec3( -2.f, -1.f, 0.f );

	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; vertIndex++ )
	{
		m_localVerts[vertIndex].m_color = Rgba8( 102, 153, 204 );
	}
}

void PlayerShip::UpdateFromKeyboard( float deltaSeconds )
{
	/*
	if (g_app->wasJustPressed(' '))
	{
	FireBullet();
	}

	if (g_app->isKeyDown('A')
	{
	orientationDegrees += playerTrnSpeedDegreesPerSecond * deltaSeconds
	}
	*/
}

void PlayerShip::BounceOffWalls()
{

}

void PlayerShip::Respawn()
{

}
