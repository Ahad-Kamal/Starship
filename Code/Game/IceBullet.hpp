#pragma once
#include "Game/Bullet.hpp"

class IceBullet : public Bullet
{
public:
	IceBullet( Game* owner, Vec2 const& startPos );
	~IceBullet() = default;

private:
	virtual void InitializeLocalVerts() override;
};