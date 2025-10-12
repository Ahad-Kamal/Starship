#pragma once
#include "Game/Asteroid.hpp"

class FieryAsteroid : public Asteroid
{
public:
	FieryAsteroid( Game* owner, Vec2 const& startPos );
	~FieryAsteroid() = default;

private:
	void ChangeColor();	
};