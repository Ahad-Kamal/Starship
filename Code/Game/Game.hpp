#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

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

extern RandomNumberGenerator* g_rng;

constexpr float MAX_SCREEN_SHAKE_AMOUNT = 10.f;
constexpr float SCREEN_SHAKE_REDUCTION = 2.f;

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
	Debris* SpawnNewDebris( Vec2 const& pos, Vec2 const& vel, Rgba8 color, float scale );
	void SpawnNewDebrisCluster( int count, Vec2 const& pos, Vec2 const& clusterVelocity, Vec2 const& forwardVector, Rgba8 color, float scale );
	Vec2 GetRandomOffScreenPosition();

private:
	void UpdateEntities( float deltaSeconds );
	void UpdateCameras( float deltaSeconds );
	void RenderEntities() const;

	void CheckBulletVsAsteroid( Bullet& bullet, Asteroid& asteroid );
	void CheckBulletVsEnemies();
	void CheckBulletVsBeetle( Bullet& bullet, Beetle& beetle );
	void CheckBulletVsWasp( Bullet& bullet, Wasp& wasp );
	void CheckAsteroidVsShip( Asteroid& asteroid, PlayerShip& ship );
	void CheckEnemiesVsShips();
	void CheckBeetleVsShip( Beetle& beetle, PlayerShip& ship);
	void CheckWaspVsShip( Wasp& wasp, PlayerShip& ship );
	void CheckEnemiesVsEnemies();
	void CheckEnemyVsEnemy( Entity& enemy1, Entity& enemy2 );

	void CheckForGameOver();
	void CheckIfWaveNeedsToSpawn();
	void SpawnWave();

	bool DoEntitiesOverlap( Entity const& a, Entity const& b);
	void DrawPlayerLives() const;
	void DebugRenderEntities() const;
	void DeleteGarbageEntities();

public:
	Camera* m_worldCamera;
	Camera* m_screenCamera;

private:
	App*		m_app = nullptr;
	PlayerShip* m_playerShip = nullptr;
	Asteroid*	m_asteroids[ MAX_ASTEROIDS ] = {};
	Bullet*		m_bullets[ MAX_BULLETS ] = {};
	Beetle*		m_beetles[ MAX_BEETLES ];
	Wasp*		m_wasps[ MAX_WASPS ];
	Debris*		m_debris[ MAX_DEBRIS ];
	
	int m_waveNumber = 0;
	int m_numAsteroidsPerWave[ NUM_WAVES ] = { 2, 2, 2, 2, 2 };
	int m_numBeetlesPerWave[ NUM_WAVES ] = { 1, 3, 0, 3, 5 };
	int m_numWaspsPerWave[ NUM_WAVES ] = { 0, 0, 1, 2, 3 };
	int m_endGameTimer = 0;

	float m_screenShakeAmount = MAX_SCREEN_SHAKE_AMOUNT;
	bool m_isShaking = false;
};