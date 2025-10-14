#include "Game/IcyAsteroid.hpp"

IcyAsteroid::IcyAsteroid( Game* owner, Vec2 const& startPos )
	: Asteroid( owner, startPos )
{
	m_isIcyAsteroid = true;
	ChangeColor( fullHealthColor );
}

void IcyAsteroid::TakeDamage( int damage )
{
	m_health -= damage;

	if( m_health <= 0 )
	{
		Die();
	}
	else if( m_health == 2 )
	{
		ChangeColor( halfHealthColor );
	}
	else if( m_health == 1 )
	{
		ChangeColor( lowHealthColor );
	}
}

void IcyAsteroid::ChangeColor( Rgba8 color )
{
	m_color = color;
	for( int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
}