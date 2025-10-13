#pragma once
#include "Game/Beetle.hpp"

class IceBeetle : public Beetle
{
public:
	IceBeetle( Game* owner, Vec2 const& startingPosition );
	~IceBeetle() = default;

private:
	void ChangeColor();
};