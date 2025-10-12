#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"

class Game;

constexpr int NUM_SHIP_TRIS = 5;
constexpr int NUM_SHIP_VERTS = 3 * NUM_SHIP_TRIS;
constexpr int NUM_THRUST_TRIS = 1;
constexpr int NUM_THRUST_VERTS = 3 * NUM_THRUST_TRIS;
constexpr int NUM_SHIP_VERTS_TOTAL = NUM_SHIP_VERTS + NUM_THRUST_VERTS;

constexpr float MAX_FIRE_BULLET_COOLDOWN = 0.2f;
constexpr float MAX_ICE_BULLET_COOLDOWN = 0.2f;

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
	void ActivateThrust();
	void DeactivateThrust();
	void Respawn();

public:
	int m_lives = 4;

private:
	Vertex	m_localVerts[ NUM_SHIP_VERTS_TOTAL ];
	bool m_isTurningLeft = false;
	bool m_isTurningRight = false;
	bool m_isKeyboardThrusting = false;
	bool m_isControllerThrusting = false;
	float m_thrustFraction = 0.f;
	float m_fireBulletCooldown = MAX_FIRE_BULLET_COOLDOWN;
	float m_iceBulletCooldown = MAX_ICE_BULLET_COOLDOWN;
};
