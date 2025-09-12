#include "PlayerShip.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"

PlayerShip::PlayerShip(Vec2 const& startingPosition, Vec2 const& startingVelocity)
{
	m_position = startingPosition;
	m_velocity = startingVelocity;
}

void PlayerShip::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;

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

void PlayerShip::Render() const
{
	Vertex shipVerts[3];

	shipVerts[0].m_pos = Vec3( (m_position.x ) + 4.f, m_position.y, 0.f);
	shipVerts[1].m_pos = Vec3( (m_position.x ) - 2.f, m_position.y + 2.0f, 0.f);
	shipVerts[2].m_pos = Vec3( (m_position.x ) - 2.f, m_position.y - 2.0f, 0.f);

	shipVerts[0].m_color = Rgba8( 255, 255, 255, 255 );
	shipVerts[1].m_color = Rgba8( 0, 0, 0, 255 );
	shipVerts[2].m_color = Rgba8( 0, 127, 255, 255 );

	shipVerts[0].m_uvTexCoords = Vec2( 0.f, 0.f );
	shipVerts[1].m_uvTexCoords = Vec2( 0.f, 0.f );
	shipVerts[2].m_uvTexCoords = Vec2( 0.f, 0.f );
	
	g_engine->m_render->DrawVertexArray( 3, shipVerts );
}