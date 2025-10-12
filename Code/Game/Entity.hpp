#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Game;

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

	bool	IsAlive() const;
	bool	IsOffScreen() const;
	Vec2	GetForwardNormal() const;

public:
	Game*	m_game = nullptr;
	Vec2	m_position;
	Vec2	m_velocity;
	Rgba8	m_color;
	float	m_orientationDegrees = 0.f; // counter-clockwise starting from +x
	float	m_angualrVelocity = 0.f; // spin rate, in degrees per second
	float	m_physicsRadius = 5.f;
	float	m_cosmeticRadius = 10.f;
	int		m_health = 1;
	bool	m_isDead = false;	// gameplay idea of "dead"
	bool	m_isGarbage = false;	// code idea of "dead"
	float	m_ageSeconds = 0.f;

protected:
	void	TakeFireDamage();

protected:
	bool	m_isOnFire = false;
	bool	m_isSlow = false;
	float	m_FireTick;
	float	m_SlowTimer;
};