#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Game/Entity.hpp"

constexpr int NUM_EXPLOSION_SIDES = 32;
constexpr int NUM_EXPLOSION_TRIS = NUM_EXPLOSION_SIDES;
constexpr int NUM_EXPLOSION_VERTS = 3 * NUM_EXPLOSION_TRIS;

class Explosion : public Entity
{
public:
	Explosion( Game* owner, Vec2 const& startPos, Rgba8 color, bool explosionType ); // true if fiery, false if icy
	~Explosion() = default;

	virtual void Update(  float deltaSeconds ) override;
	virtual void Render() const override;
	virtual void Die() override;

private:
	void InitializeLocalVerts();

public:
	bool m_isFieryExplosion = false;
	bool m_isIcyExplosion = false;

private:
	Vertex m_localVerts[ NUM_EXPLOSION_VERTS ];
	float m_lifeTimeSeconds = 2.f;
};