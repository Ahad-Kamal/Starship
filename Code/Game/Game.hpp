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
class Beetle;
class Wasp;
class Debris;

//-----------------------------------------------------------------------------------------------

// bool IsAlive( Entity* entity ) const

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
	Beetle* SpawnNewRandomBeetle();
	Wasp* SpawnNewRandomWasp();
	Debris* SpawnNewDebris( Vec2 const& pos, Vec2 const& vel, Rgba8 color );
	void SpawnNewDebrisCluster( int count, Vec2 const& pos, Vec2 const& clusterVelocity, Vec2 const& forwardVector, Rgba8 color );
	//GetRandomOffScreenPosition();

private:
	void UpdateEntities( float deltaSeconds );
	void RenderEntities() const;

	void CheckBulletVsAsteroid( Bullet& bullet, Asteroid& asteroid );
	void CheckBulletVsEnemies();
	void CheckBulletVsBeetle( Bullet& bullet, Beetle& beetle );
	void CheckBulletVsWasp( Bullet& bullet, Wasp& wasp );
	void CheckAsteroidVsShip( Asteroid& asteroid, PlayerShip& ship );
	void CheckEnemiesVsShips();
	void CheckBeetleVsShip( Beetle& beetle, PlayerShip& ship);
	void CheckWaspVsShip( Wasp& wasp, PlayerShip& ship );

	bool DoEntitiesOverlap( Entity const& a, Entity const& b);
	void DebugRenderEntities() const;
	void DeleteGarbageEntities();

private:
	App*		m_app = nullptr;
	Camera*		m_gameCamera = nullptr;
	PlayerShip* m_playerShip = nullptr;
	Asteroid*	m_asteroids[ MAX_ASTEROIDS ] = {};
	Bullet*		m_bullets[ MAX_BULLETS ] = {};
	Beetle*		m_beetles[ MAX_BEETLES ];
	Wasp*		m_wasps[ MAX_WASPS ];
	Debris*		m_debris[ MAX_DEBRIS ];
};