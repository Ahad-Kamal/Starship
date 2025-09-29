#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex.hpp"

class Game;
class PlayerShip;
constexpr int NUM_WASP_TRIS = 8;
constexpr int NUM_WASP_VERTS = 3 * NUM_WASP_TRIS;

class Wasp : public Entity
{
public:
	Wasp( Game* owner, Vec2 const& startingPosition );
	~Wasp() = default;

	virtual void Update( float deltaSeconds ) override;
	void Update( float deltaSeconds, PlayerShip const& ship );
	virtual void Render() const override;

private:
	void InitializeLocalVerts();

private:
	Vertex m_localVerts[ NUM_WASP_VERTS ];
};