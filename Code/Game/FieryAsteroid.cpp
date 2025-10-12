#include "Game/FieryAsteroid.hpp"

FieryAsteroid::FieryAsteroid( Game* owner, Vec2 const& startPos )
	: Asteroid( owner, startPos )
{
	ChangeColor();
}

void FieryAsteroid::ChangeColor()
{
	m_color = Rgba8( 128, 0, 0 );
	for( int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
}