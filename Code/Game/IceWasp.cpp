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

	m_localVerts[ 9 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 10 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 11 ].m_color = Rgba8( 51, 153, 255 );

	m_localVerts[ 12 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 13 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 14 ].m_color = Rgba8( 51, 153, 255 );

	m_localVerts[ 15 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 16 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 17 ].m_color = Rgba8( 51, 153, 255 );

	m_localVerts[ 18 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 19 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 20 ].m_color = Rgba8( 51, 153, 255 );
}