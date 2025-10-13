#pragma once
#include "Game/Wasp.hpp"

class IceWasp : public Wasp
{
public:
	IceWasp( Game* owner, Vec2 const& startingPosition );
	~IceWasp() = default;

private:
	void ChangeColor();
};