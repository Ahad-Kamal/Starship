#pragma once
#include "Game/Asteroid.hpp"
#include "Engine/Core/Rgba8.hpp"

class FieryAsteroid : public Asteroid
{
public:
	FieryAsteroid( Game* owner, Vec2 const& startPos );
	~FieryAsteroid() = default;

	virtual void TakeDamage( int damage ) override;

private:
	void ChangeColor( Rgba8 color );	

private:
	Rgba8 fullHealthColor = Rgba8( 128, 0, 0 );
	Rgba8 halfHealthColor = Rgba8( 179, 0, 0 );
	Rgba8 lowHealthColor = Rgba8( 230, 0, 0 );
};	