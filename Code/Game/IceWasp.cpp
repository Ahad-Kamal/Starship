#include "Game/IceWasp.hpp"

IceWasp::IceWasp( Game* owner, Vec2 const& startingPosition )
	: Wasp( owner, startingPosition )
{
	m_isIceWasp = true;
	ChangeColor();
}

void IceWasp::ChangeColor()
{
	m_color = Rgba8( 128, 229, 255 );
	for( int vertIndex = 0; vertIndex < NUM_WASP_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
}