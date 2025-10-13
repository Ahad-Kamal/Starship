#pragma once
#include "Game/Beetle.hpp"

class FireBeetle : public Beetle
{
public:
	FireBeetle( Game* owner, Vec2 const& startingPosition );
	~FireBeetle() = default;

private:
	void ChangeColor();
};