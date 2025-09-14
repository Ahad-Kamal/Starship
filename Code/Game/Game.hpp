#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
class App;
class Camera;
class PlayerShip;
class Asteroid;
class Bullet;
class Entity;

//-----------------------------------------------------------------------------------------------
class Game
{
public:
	Game( App* owner );
	~Game();
	void Startup();
	void Update( float deltaSeconds );
	void Render() const;
	void Shutdown();

	Asteroid* SpawnRandomAsteroid();
	Bullet* SpawnBullet(Vec2 const& pos, float forwardDegrees);

private:
	void UpdateEntities( float deltaSeconds );
	void RenderEntities() const;
	void CheckBulletsVsAsteroids();
	void CheckBulletVsAsteroid( Bullet& bullet, Asteroid& asteroid );
	void CheckAsteroidsVsShips();
	void CheckAsteroidVsShip( Asteroid& asteroid, PlayerShip& ship );
	void DoEntitiesOverlap( Entity const& a, Entity const& b);
	void DebugRenderEntities() const;
	void DeleteGarbageEntities();

private:
	App*		m_app = nullptr;
	Camera*		m_gameCamera = nullptr;
	PlayerShip* m_playerShip = nullptr;
	Asteroid*	m_asteroids[MAX_ASTEROIDS] = {};
	Bullet*		m_bullets[MAX_BULLETS] = {};
};