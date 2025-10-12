#pragma once
#include "Game/Bullet.hpp"

class FireBullet : public Bullet
{
public:
	FireBullet( Game* owner, Vec2 const& startPos );
	~FireBullet() = default;

private:
	virtual void InitializeLocalVerts() override;
};