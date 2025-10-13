#include "Game/FireWasp.hpp"

FireWasp::FireWasp( Game* owner, Vec2 const& startingPosition )
	: Wasp( owner, startingPosition )
{
	m_isFireWasp = true;
	ChangeColor();
}

void FireWasp::ChangeColor()
{
	m_color = Rgba8( 255, 128, 128 );
	for( int vertIndex = 0; vertIndex < NUM_WASP_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}
}

