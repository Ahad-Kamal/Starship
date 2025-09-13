#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"


// Ring is made up of 12 trapezoids
void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color)
{
	float halfThickness = 0.5f * thickness;
	float innerRadius = radius - halfThickness;
	float outerRadius = radius + halfThickness;
	constexpr int NUM_SIDES = 32;
	constexpr int NUM_TRIS = 2 * NUM_SIDES;	// Each side is a trapezoid
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	constexpr float DEGREES_PER_SIDE = 360.f / static_cast<float>( NUM_SIDES );
	Vertex verts[ NUM_VERTS ];

	for (int sideNum = 0; sideNum < NUM_SIDES; sideNum++)
	{
		// Compute angles
		float startDegrees =  DEGREES_PER_SIDE * static_cast<float>( sideNum );
		float endDegrees = DEGREES_PER_SIDE * static_cast<float>( sideNum + 1 );
		float cosStart = CosDegrees( startDegrees );
		float sinStart = SinDegrees( startDegrees );
		float cosEnd = CosDegrees( endDegrees );
		float sinEnd = SinDegrees( endDegrees );

		// Compute Inner and Outer Positions
		Vec3 innerStartPos = Vec3( center.x + innerRadius * cosStart, center.y + innerRadius * sinStart, 0.f);
		Vec3 outerStartPos = Vec3( center.x + outerRadius * cosStart, center.y + outerRadius * sinStart, 0.f );
		Vec3 outerEndPos = Vec3( center.x + outerRadius * cosEnd, center.y + outerRadius * sinEnd, 0.f );
		Vec3 innerEndPos = Vec3( center.x + innerRadius * cosEnd, center.y + innerRadius * sinEnd, 0.f );

		// Trapezoid is made up of two triangles. Triangles are ABC and DEF
		// A is inner end, B is inner start, C is outer start
		// D is inner end, E is outer start, F is outer end
		int vertIndexA = 6 * sideNum;
		int vertIndexB = (6 * sideNum) + 1;
		int vertIndexC = (6 * sideNum) + 2;
		int vertIndexD = (6 * sideNum) + 3;
		int vertIndexE = (6 * sideNum) + 4;
		int vertIndexF = (6 * sideNum) + 5;

		verts[ vertIndexA ].m_pos = innerEndPos;
		verts[ vertIndexB ].m_pos = innerStartPos;
		verts[ vertIndexC ].m_pos = outerStartPos;

		verts[ vertIndexA ].m_color = color;
		verts[ vertIndexB ].m_color = color;
		verts[ vertIndexC ].m_color = color;

		verts[ vertIndexD ].m_pos = innerEndPos;
		verts[ vertIndexE ].m_pos = outerStartPos;
		verts[ vertIndexF ].m_pos = outerEndPos;

		verts[ vertIndexD ].m_color = color;
		verts[ vertIndexE ].m_color = color;
		verts[ vertIndexF ].m_color = color;
	}

	g_engine->m_render->DrawVertexArray( NUM_VERTS, verts);
}
