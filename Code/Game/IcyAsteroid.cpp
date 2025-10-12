#include "Game/IcyAsteroid.hpp"

IcyAsteroid::IcyAsteroid( Game* owner, Vec2 const& startPos )
	: Asteroid( owner, startPos )
{
	ChangeColor();
}

void IcyAsteroid::ChangeColor()
{
	Rgba8 iceAsteroidColor = Rgba8( 0, 102, 153 );
	for( int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = iceAsteroidColor;
	}
}