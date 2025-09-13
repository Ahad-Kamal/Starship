#pragma once
#include "Engine/Math/Vec2.hpp"

class Game;

//-----------------------------------------------------------------------------------------------
class PlayerShip
{
public:
	PlayerShip( Game* owner, Vec2 const& startingPosition );
	PlayerShip( Game* owner, Vec2 const& startingPosition, Vec2 const& startingVelocity );
	~PlayerShip() = default;
	void Update( float deltaSeconds );
	void Render() const;

public:
	Vec2	m_position;
	Vec2	m_velocity;
	float	m_health;
};