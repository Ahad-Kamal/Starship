#include "Game/IceBeetle.hpp"

IceBeetle::IceBeetle( Game* owner, Vec2 const& startingPosition )
	: Beetle( owner, startingPosition )
{
	m_isIceBeetle = true;
	ChangeColor();
	m_spiralAngle = -30.f;
}

void IceBeetle::ChangeColor()
{
	m_color = Rgba8( 128, 229, 255 );
	for( int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}

	m_localVerts[ 3 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 4 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 5 ].m_color = Rgba8( 51, 153, 255 );

	m_localVerts[ 6 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 7 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 8 ].m_color = Rgba8( 51, 153, 255 );

	m_localVerts[ 12 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 13 ].m_color = Rgba8( 51, 153, 255 );
	m_localVerts[ 14 ].m_color = Rgba8( 51, 153, 255 );
}