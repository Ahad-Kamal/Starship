#pragma once
#include "Game/Asteroid.hpp"
#include "Engine/Core/Rgba8.hpp"

class IcyAsteroid : public Asteroid
{
public:
	IcyAsteroid( Game* owner, Vec2 const& startPos );
	~IcyAsteroid() = default;

	virtual void TakeDamage( int damage ) override;

private:
	void ChangeColor( Rgba8 color );

private:
	Rgba8 fullHealthColor = Rgba8( 0, 96, 128 );
	Rgba8 halfHealthColor = Rgba8( 0, 134, 179 );
	Rgba8 lowHealthColor = Rgba8( 0, 172, 230 );
};