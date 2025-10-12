#include "Game/FireBullet.hpp"
#include "Game/Bullet.hpp"

FireBullet::FireBullet( Game* owner, Vec2 const& startPos )
	: Bullet( owner, startPos )
{
	InitializeLocalVerts();
	m_isFireBullet = true;
}

void FireBullet::InitializeLocalVerts()
{
	// Bullet
	m_localVerts[ 0 ].m_pos = Vec3( 0.5f, 0.f, 0.f );
	m_localVerts[ 1 ].m_pos = Vec3( 0.f, 0.5f, 0.f );
	m_localVerts[ 2 ].m_pos = Vec3( 0.f, -0.5f, 0.f );
	m_localVerts[ 0 ].m_color = Rgba8( 255, 255, 0, 255 );
	m_localVerts[ 1 ].m_color = Rgba8( 255, 128, 0, 255 );
	m_localVerts[ 2 ].m_color = Rgba8( 255, 128, 0, 255 );

	// Trail
	m_localVerts[ 3 ].m_pos = Vec3( 0.f, -0.5f, 0.f );
	m_localVerts[ 4 ].m_pos = Vec3( 0.f, 0.5f, 0.f );
	m_localVerts[ 5 ].m_pos = Vec3( -3.f, 0.f, 0.f );
	m_localVerts[ 3 ].m_color = Rgba8( 255, 128, 0, 255 );
	m_localVerts[ 4 ].m_color = Rgba8( 255, 128, 0, 255 );
	m_localVerts[ 5 ].m_color = Rgba8( 255, 0, 0, 0 );
}

