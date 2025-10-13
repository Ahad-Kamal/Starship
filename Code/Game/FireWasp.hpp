#pragma once
#include "Game/Wasp.hpp"

class FireWasp : public Wasp
{
public:
	FireWasp( Game* owner, Vec2 const& startingPosition );
	~FireWasp() = default;

private:
	void ChangeColor();
};