#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"

class Game;
constexpr int NUM_SHIP_TRIS = 5;
constexpr int NUM_SHIP_VERTS = 3 * NUM_SHIP_TRIS;

void createFakePlayerShip( Vertex verts[], float transparency );

//----------------------------------------------------------------------------------------------
class PlayerShip : public Entity
{
public:
	PlayerShip( Game* owner, Vec2 const& startingPosition );
	PlayerShip( Game* owner, Vec2 const& startingPosition, Vec2 const& startingVelocity );
	~PlayerShip() = default;

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	virtual void Die() override;

private:
	void InitializeLocalVerts();
	void UpdateFromKeyboard( float deltaSeconds );
	void UpdateFromController();
	void BounceOffWalls();
	void Respawn();

public:
	int m_lives = 4;

private:
	Vertex	m_localVerts[ NUM_SHIP_VERTS ];
	bool m_isTurningLeft = false;
	bool m_isTurningRight = false;
	bool m_isThrusting = false;
	float m_thrustFraction = 0.f;
};
