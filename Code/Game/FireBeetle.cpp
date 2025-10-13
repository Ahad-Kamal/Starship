#include "Game/FireBeetle.hpp"

FireBeetle::FireBeetle( Game* owner, Vec2 const& startingPosition )
	: Beetle( owner, startingPosition )
{
	m_isFireBeetle = true;
	ChangeColor();
}

void FireBeetle::ChangeColor()
{
	m_color = Rgba8( 255, 128, 128 );
	for( int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
}

