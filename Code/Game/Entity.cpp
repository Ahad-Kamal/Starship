#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/MathUtils.hpp"

Entity::Entity( Game* owner, Vec2 const& startPos )
	: m_game( owner )
	, m_position( startPos )
{
}

Entity::~Entity()
{

}

void Entity::DebugRender() const
{
	//float lineThickness = 0.2f;

	//float velocityVector = m_velocity.GetLength();

	DebugDrawRing( m_position, m_physicsRadius, DEBUG_LINE_THICKNESS, Rgba8( 0, 255, 255 ) );
	DebugDrawRing( m_position, m_cosmeticRadius, DEBUG_LINE_THICKNESS, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( m_position, m_position + m_velocity, DEBUG_LINE_THICKNESS, Rgba8( 255, 255, 0 ) );
}

bool Entity::isOffScreen() const
{
	if( m_position.x > WORLD_SIZE_X + m_cosmeticRadius )
	{
		return true;
	}
	else if( m_position.y > WORLD_SIZE_Y + m_cosmeticRadius )
	{
		return true;
	}
	else if( m_position.x < -m_cosmeticRadius )
	{
		return true;
	}
	else if( m_position. y < -m_cosmeticRadius )
	{
		return true;
	}

	return false;
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) );
}

