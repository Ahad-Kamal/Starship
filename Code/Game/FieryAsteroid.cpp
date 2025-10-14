#include "Game/FieryAsteroid.hpp"

FieryAsteroid::FieryAsteroid( Game* owner, Vec2 const& startPos )
	: Asteroid( owner, startPos )
{
	m_isFieryAsteroid = true;
	ChangeColor( fullHealthColor );
}

void FieryAsteroid::TakeDamage( int damage )
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

void FieryAsteroid::ChangeColor( Rgba8 color )
{
	m_color = color;
	for( int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
}