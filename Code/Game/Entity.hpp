#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Game;

constexpr float FIRE_DAMAGE_COOLDOWN = 120.f;
constexpr int FIRE_DAMAGE_TICK = 3;

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
	void	ResetFireTick();

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
	bool	m_isOnFire = false;
	bool	m_isSlow = false;

protected:
	void	TakeFireDamage();

protected:
	float	m_fireCooldown = FIRE_DAMAGE_COOLDOWN;
	float	m_fireTick = FIRE_DAMAGE_TICK;
	float	m_slowTimer;
};