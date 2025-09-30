#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Game/Entity.hpp"

class Game;
struct Vec2;
struct Rgba8;

constexpr int NUM_DEBRIS_SIDES = 10;
constexpr int NUM_DEBRIS_TRIS = NUM_DEBRIS_SIDES;
constexpr int NUM_DEBRIS_VERTS = 3 * NUM_DEBRIS_TRIS;

class Debris : public Entity
{
public:
	//Debris( Game* owner, Vec2 const& startingPos, Vec2 const& vel, float radius, Rgba8 color, float lifetimeSeconds );
	Debris( Game* owner, Vec2 const& startingPos, Vec2 const& vel, Rgba8 color );
	~Debris() = default;

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override; 
	virtual void Die() override;

private:
	void InitializeLocalVerts();

public:
	Vertex m_localVerts[ NUM_DEBRIS_VERTS ];
	float m_lifeTimeSeconds = 2.f;
};