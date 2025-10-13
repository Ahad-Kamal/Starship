#include "Game/IceBeetle.hpp"

IceBeetle::IceBeetle( Game* owner, Vec2 const& startingPosition )
	: Beetle( owner, startingPosition )
{
	m_isIceBeetle = true;
	ChangeColor();
}

void IceBeetle::ChangeColor()
{
	m_color = Rgba8( 128, 229, 255 );
	for( int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
}