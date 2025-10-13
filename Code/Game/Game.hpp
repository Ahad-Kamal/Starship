#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Vertex.hpp"

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
class Explosion;

//-----------------------------------------------------------------------------------------------

extern RandomNumberGenerator* g_rng;

constexpr float MAX_SCREEN_SHAKE_AMOUNT = 5.f;
constexpr float SCREEN_SHAKE_REDUCTION = 5.f;
constexpr int STAR_TRIS = 2;
constexpr int VERTS_PER_STAR = 3 * STAR_TRIS;
constexpr int NUM_STAR_VERTS = VERTS_PER_STAR * MAX_STARS;

class Game
{
public:
	Game( App* owner );
	~Game();
	void Startup();
	void Update( float deltaSeconds );
	void Render() const;
	void Shutdown();

	Asteroid* SpawnRandomFieryAsteroid();
	Asteroid* SpawnRandomIcyAsteroid();

	Bullet* SpawnFireBullet(Vec2 const& pos, float forwardDegrees);
	Bullet* SpawnIceBullet(Vec2 const& pos, float forwardDegrees);

	Beetle* SpawnNewRandomBeetle();
	Beetle* SpawnNewRandomFireBeetle();
	Beetle* SpawnNewRandomIceBeetle();

	Wasp* SpawnNewRandomWasp();
	Wasp* SpawnNewRandomFireWasp();
	Wasp* SpawnNewRandomIceWasp();

	Debris* SpawnNewDebris( Vec2 const& pos, Vec2 const& vel, Rgba8 color, float scale );
	void SpawnNewDebrisCluster( int count, Vec2 const& pos, Vec2 const& clusterVelocity, Vec2 const& forwardVector, Rgba8 color, float scale );
	Explosion* SpawnNewExplosion( Vec2 const& pos, Rgba8 color );

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

	void ClampCamera( Vec2& minView, Vec2& maxView );
	void CheckForGameOver();
	void CheckIfWaveNeedsToSpawn();
	void SpawnWave();
	bool DoEntitiesOverlap( Entity const& a, Entity const& b);
	void DrawPlayerLives() const;
	void CreateStarfield();
	void RenderStars() const;

	void DebugRenderEntities() const;
	void DebugDrawWorldBounds() const;
	void DeleteGarbageEntities();

public:
	Camera* m_worldCamera;
	Camera* m_screenCamera;
	bool m_isShaking = false;

private:
	App*		m_app = nullptr;
	PlayerShip* m_playerShip = nullptr;
	Asteroid*	m_asteroids[ MAX_ASTEROIDS ] = {};
	Bullet*		m_bullets[ MAX_BULLETS ] = {};
	Beetle*		m_beetles[ MAX_BEETLES ];
	Wasp*		m_wasps[ MAX_WASPS ];
	Debris*		m_debris[ MAX_DEBRIS ];
	Explosion*	m_explosions[ MAX_EXPLOSIONS ];
	
	Vertex m_starVerts[ NUM_STAR_VERTS ];
	Vertex m_starVertsNear[ NUM_STAR_VERTS ];
	Vertex m_starVertsFar[ NUM_STAR_VERTS ];

	int m_waveNumber = 0;
	int m_numFieryAsteroidsPerWave[ NUM_WAVES ] = { 6, 6, 6, 6, 6 };
	int m_numIcyAsteroidsPerWave[ NUM_WAVES ] = { 6, 6, 6, 6, 6 };
	int m_numBeetlesPerWave[ NUM_WAVES ] = { 1, 2, 0, 3, 5 };
	int m_numFireBeetlesPerWave[ NUM_WAVES ] = { 1, 2, 0, 3, 5 };
	int m_numIceBeetlesPerWave[ NUM_WAVES ] = { 1, 2, 0, 3, 5 };
	int m_numWaspsPerWave[ NUM_WAVES ] = { 0, 0, 1, 2, 3 };
	int m_numFireWaspsPerWave[ NUM_WAVES ] = { 0, 0, 1, 2, 3 };
	int m_numIceWaspsPerWave[ NUM_WAVES ] = { 0, 0, 1, 2, 3 };
	int m_endGameTimer = 0;

	float m_screenShakeAmount = MAX_SCREEN_SHAKE_AMOUNT;
};