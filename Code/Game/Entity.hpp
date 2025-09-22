#pragma once
#include "Engine/Math/Vec2.hpp"

class Game;
struct Vec2;

class Entity
{
public:
	Entity( Game* owner, Vec2 const& startPos );
	virtual ~Entity();

	virtual void Update( float deltaSeconds ) = 0;
	virtual void Render() const = 0;
	virtual void DebugRender() const;
	virtual void TakeDamage( int damage );
	virtual void Die();

	bool	IsAlive();
	bool	IsOffScreen() const;
	Vec2	GetForwardNormal() const;

public:
	Game*	m_game = nullptr;
	Vec2	m_position;
	Vec2	m_velocity;
	float	m_orientationDegrees = 0.f; // counter-clockwise starting from +x
	float	m_angualrVelocity = 0.f; // spin rate, in degrees per second
	float	m_physicsRadius = 5.f;
	float	m_cosmeticRadius = 10.f;
	int		m_health = 1;
	bool	m_isDead = false;	// gameplay idea of "dead"
	bool	m_isGarbage = false;	// code idea of "dead"
	float	m_ageSeconds = 0.f;
};