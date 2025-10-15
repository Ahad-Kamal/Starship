#include "Game/FireBeetle.hpp"

FireBeetle::FireBeetle( Game* owner, Vec2 const& startingPosition )
	: Beetle( owner, startingPosition )
{
	m_isFireBeetle = true;
	ChangeColor();
	m_spiralAngle = 30.f;
}

void FireBeetle::ChangeColor()
{
	m_color = Rgba8( 255, 128, 128 );
	for( int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; vertIndex++ )
	{
		m_localVerts[ vertIndex ].m_color = m_color;
	}

	m_localVerts[ 3 ].m_color = Rgba8( 255, 51, 51 );
	m_localVerts[ 4 ].m_color = Rgba8( 255, 51, 51 );
	m_localVerts[ 5 ].m_color = Rgba8( 255, 51, 51 );

	m_localVerts[ 6 ].m_color = Rgba8( 255, 51, 51 );
	m_localVerts[ 7 ].m_color = Rgba8( 255, 51, 51 );
	m_localVerts[ 8 ].m_color = Rgba8( 255, 51, 51 );

	m_localVerts[ 12 ].m_color = Rgba8( 255, 51, 51 );
	m_localVerts[ 13 ].m_color = Rgba8( 255, 51, 51 );
	m_localVerts[ 14 ].m_color = Rgba8( 255, 51, 51 );
}

