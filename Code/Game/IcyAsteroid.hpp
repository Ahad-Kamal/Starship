#pragma once
#include "Game/Asteroid.hpp"

class IcyAsteroid : public Asteroid
{
public:
	IcyAsteroid( Game* owner, Vec2 const& startPos );
	~IcyAsteroid() = default;

private:
	void ChangeColor();
};