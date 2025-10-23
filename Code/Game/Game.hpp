#pragma once
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
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
extern RandomNumberGenerator* g_rng;


//-----------------------------------------------------------------------------------------------
constexpr float MAX_SCREEN_SHAKE_AMOUNT = 5.f;
constexpr float SCREEN_SHAKE_REDUCTION = 5.f;
constexpr int STAR_TRIS = 2;
constexpr int VERTS_PER_STAR = 3 * STAR_TRIS;
constexpr int NUM_STAR_VERTS = VERTS_PER_STAR * MAX_STARS;


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

	Asteroid* SpawnNewRandomFieryAsteroid();
	Asteroid* SpawnNewRandomIcyAsteroid();

	Bullet* SpawnNewFireBullet(Vec2 const& pos, float forwardDegrees);
	Bullet* SpawnNewIceBullet(Vec2 const& pos, float forwardDegrees);

	Beetle* SpawnNewRandomBeetle();
	Beetle* SpawnNewRandomIceBeetle();
	Beetle* SpawnNewRandomFireBeetle();

	Wasp* SpawnNewRandomWasp();
	Wasp* SpawnNewRandomFireWasp();
	Wasp* SpawnNewRandomIceWasp();

	Debris* SpawnNewDebris( Vec2 const& pos, Vec2 const& vel, Rgba8 color, float scale );
	void SpawnNewDebrisCluster( int count, Vec2 const& pos, Vec2 const& clusterVelocity, Vec2 const& forwardVector, Rgba8 color, float scale );
	Explosion* SpawnNewExplosion( Vec2 const& pos, Rgba8 color, bool explosionType ); // true if fiery, false if icy
	
	Vec2 GetRandomOffScreenPosition() const;
	void AddCameraShake( float shakeAmount );

private:
	void UpdateStates();
	void UpdateAttractMode( float deltaSeconds );
	void RenderAttractMode() const;

	void UpdateEntities( float deltaSeconds );
	void UpdateCameras( float deltaSeconds );
	void RenderEntities() const;

	void CheckBulletVsEnemies();
	void CheckBulletVsAsteroid( Bullet& bullet, Asteroid& asteroid );
	void CheckBulletVsBeetle( Bullet& bullet, Beetle& beetle );
	void CheckBulletVsWasp( Bullet& bullet, Wasp& wasp );

	void CheckShipsVsEnemies();
	void CheckShipVsAsteroid( Asteroid& asteroid, PlayerShip& ship );
	void CheckShipVsBeetle( Beetle& beetle, PlayerShip& ship);
	void CheckShipVsWasp( Wasp& wasp, PlayerShip& ship );

	void CheckEnemiesVsEnemies();
	void CheckEnemyVsEnemy( Entity& enemy1, Entity& enemy2 );

	void CheckExplosionsVsEnemies();
	void CheckExplosionsVsBeetle( Explosion& explosion, Beetle& beetle );
	void CheckExplosionsVsWasp( Explosion& explosion, Wasp& wasp );

	void ClampCamera( Vec2& minView, Vec2& maxView );
	void ShakeCamera( float deltaSeconds );

	void CheckForGameOver();
	void CheckIfWaveNeedsToSpawn();
	void SpawnWave();

	bool DoEntitiesOverlap( Entity const& a, Entity const& b);
	void DrawPlayerLives() const;

	void CreateStarfield();
	void RenderStars() const;

	void UpdateKeyboardInput();
	void UpdateControllerInput();

	void InitializeStartTriangleVerts();

	void DebugRenderEntities() const;
	void DebugDrawWorldBounds() const;
	void DeleteGarbageEntities();

public:
	Camera* m_worldCamera;
	Camera* m_screenCamera;

	GameState m_currentState = GAME_STATE_INVALID;
	GameState m_nextState = GAME_STATE_ATTRACT;


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

	Vertex m_startVerts[ 3 ];
	Vertex m_fakeShipVerts[ NUM_SHIP_VERTS ];
	float m_startAlpha = 255.f;
	float m_shipRotation = 0.f;
	float m_time = 0.f;
	Rgba8 m_clearColor = Rgba8( 0, 0, 0, 1 );
	SoundPlaybackID m_music;

	int m_waveNumber = 0;
	int m_numFieryAsteroidsPerWave[ NUM_WAVES ] = { 6, 6, 4, 4, 4 };
	int m_numIcyAsteroidsPerWave[ NUM_WAVES ]	= { 6, 6, 4, 4, 4 };
	int m_numBeetlesPerWave[ NUM_WAVES ]		= { 1, 2, 0, 2, 5 };
	int m_numFireBeetlesPerWave[ NUM_WAVES ]	= { 1, 2, 0, 2, 5 };
	int m_numIceBeetlesPerWave[ NUM_WAVES ]		= { 1, 2, 0, 2, 5 };
	int m_numWaspsPerWave[ NUM_WAVES ]			= { 0, 0, 1, 1, 3 };
	int m_numFireWaspsPerWave[ NUM_WAVES ]		= { 0, 0, 1, 1, 3 };
	int m_numIceWaspsPerWave[ NUM_WAVES ]		= { 0, 0, 1, 1, 3 };
	int m_endGameTimer = 0;

	float m_screenShakeAmount = 0.f;
	bool m_isShaking = false;
	bool m_debugDraw = false;
};