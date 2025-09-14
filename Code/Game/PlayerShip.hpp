#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"

class Game;
constexpr int NUM_SHIP_TRIS = 5;
constexpr int NUM_SHIP_VERTS = 3 * NUM_SHIP_TRIS;

//-----------------------------------------------------------------------------------------------
class PlayerShip : public Entity
{
public:
	PlayerShip( Game* owner, Vec2 const& startingPosition );
	PlayerShip( Game* owner, Vec2 const& startingPosition, Vec2 const& startingVelocity );
	~PlayerShip() = default;

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

private:
	void InitializeLocalVerts();
	void UpdateFromKeyboard( float deltaSeconds );
	void BounceOffWalls();
	void Respawn();

private:
	Vertex	m_localVerts[NUM_SHIP_VERTS];
	bool m_isTurningLeft = false;
	bool m_isTurningRight = false;
	bool m_isThrusting = false;
};