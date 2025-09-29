#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Game/Entity.hpp"

class Game;
class PlayerShip;
constexpr int NUM_BEETLE_TRIS = 6;
constexpr int NUM_BEETLE_VERTS = 3 * NUM_BEETLE_TRIS;

class Beetle : public Entity
{
public:
	Beetle( Game* owner, Vec2 const& startingPosition );
	~Beetle() = default;

	virtual void Update( float deltaSeconds ) override;
	void Update( float deltaSeconds, PlayerShip const& ship );
	virtual void Render() const override;

private:
	void InitializeLocalVerts();

private:
	Vertex m_localVerts[ NUM_BEETLE_VERTS ];	
};
