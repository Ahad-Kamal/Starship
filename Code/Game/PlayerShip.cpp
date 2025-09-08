#include "PlayerShip.hpp"
#include "Engine/Core/Vertex.hpp"

PlayerShip::PlayerShip(Vec2 const& startingPosition, Vec2 const& startingVelocity)
{
	m_position = startingPosition;
	m_velocity = startingVelocity;
}

void PlayerShip::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
}

void PlayerShip::Render() const
{
	Vertex shipVerts[3];

	shipVerts[0].m_pos = Vec3( m_position.x + 4.f, m_position.y + 0.f, 0.f);
	// [1]
	// [2]

	shipVerts[0].m_color = Rgba8( 255, 255, 255, 255 );
	// [1]
	// [2]

	shipVerts[0].m_uvTexCoords = Vec2( 0.f, 0.f );
	// [1]
	// [2]
	
	// g_engine->m_render->DrawVertexArray( 3, shipVerts );
}