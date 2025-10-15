#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Game/Entity.hpp"

class Game;
class PlayerShip;
constexpr int NUM_WASP_TRIS = 10;
constexpr int NUM_WASP_VERTS = 3 * NUM_WASP_TRIS;

class Wasp : public Entity
{
public:
	Wasp( Game* owner, Vec2 const& startingPosition );
	~Wasp() = default;

	virtual void Update( float deltaSeconds ) override;
	void Update( float deltaSeconds, PlayerShip const& ship );
	virtual void Render() const override;

protected:
	void InitializeLocalVerts();

public:
	bool m_isFireWasp = false;
	bool m_isIceWasp = false;

protected:
	Vertex m_localVerts[ NUM_WASP_VERTS ];

};