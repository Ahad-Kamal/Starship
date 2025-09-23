#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Entity.hpp"
#include "Game/Debris.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"

Debris::Debris( Game* owner, Vec2 const& startingPos, Vec2 const& vel, float radius, Rgba8 color, float lifetimeSeconds )
	: Entity( owner, startingPos )
{

}

void Debris::Update( float deltaSeconds )
{
	m_ageSeconds += deltaSeconds;

	if( m_ageSeconds >= m_lifeTimeSeconds)
	{
		Die();
	}
}

void Debris::Render() const
{
	Vertex tempDebrisVerts[ NUM_DEBRIS_VERTS ];
	for( int i = 0; i < NUM_DEBRIS_VERTS; i++ )
	{
		tempDebrisVerts[i] = m_localVerts[i];
		float alphaFloat = RangeMapClamped( m_ageSeconds, 0.f, m_lifeTimeSeconds, 127.f, 0.f );
		tempDebrisVerts[i].m_color.a = static_cast<unsigned char>( alphaFloat );
	}

	TransformVertexArrayXY3D( NUM_DEBRIS_VERTS, tempDebrisVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( NUM_DEBRIS_VERTS, tempDebrisVerts ); 
}
