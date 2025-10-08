#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/App.hpp"
#include "Game/Bullet.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Beetle.hpp"
#include "Game/Wasp.hpp"
#include "Game/Debris.hpp"

RandomNumberGenerator* g_rng = nullptr;

Game::Game(App* owner)
{
	m_app = owner;
	Startup();
}

Game::~Game()
{
	
}

void Game::Startup()
{
	m_worldCamera = new Camera();
	m_screenCamera = new Camera();

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f) , Vec2( 200.f, 100.f) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f) , Vec2( 1600.f, 800.f) );
	
	Vec2 worldCenter( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f );
	m_playerShip = new PlayerShip( this, worldCenter );

	SpawnWave();
}

void Game::Update(float deltaSeconds)
{

	UpdateEntities( deltaSeconds );
	CheckBulletVsEnemies();
	CheckEnemiesVsShips();
	CheckEnemiesVsEnemies();
	DeleteGarbageEntities();
	CheckIfWaveNeedsToSpawn();
	CheckForGameOver();

}

void Game::Render() const
{
	RenderEntities();

	DrawPlayerLives();

	if ( m_app->m_debugDraw )
	{
		DebugRenderEntities();
	}
}

void Game::Shutdown()
{
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet*& bullet = m_bullets[ bulletIndex ];
		if( bullet )
		{
			delete bullet;
			bullet = nullptr;
		}
	}

	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid*& asteroid = m_asteroids[ asteroidIndex ];
		if( asteroid )
		{
			delete asteroid;
			asteroid = nullptr;
		}
	}

	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle*& beetle = m_beetles[ beetleIndex ];
		if( beetle  )
		{
			delete beetle;
			beetle = nullptr;
		}
	}

	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp*& wasp = m_wasps[ waspIndex ];
		if( wasp )
		{
			delete wasp;
			wasp = nullptr;
		}
	}
	
	if( m_playerShip )
	{
		delete m_playerShip;
		m_playerShip = nullptr;
	}

	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris*& debris = m_debris[ debrisIndex ];
		if( debris )
		{
			delete debris;
			debris = nullptr;
		}
	}
}

Asteroid* Game::SpawnRandomAsteroid()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition();

	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid*& asteroid = m_asteroids[ asteroidIndex ];
		if( !asteroid )
		{
			asteroid = new Asteroid( this, spawnPosition );
			asteroid->m_angualrVelocity = g_rng->RollRandomFloatInRange( -300.f, 300.f );
			asteroid->m_orientationDegrees = g_rng->RollRandomFloatInRange( 0.f, 360.f );
			float driftAngleDegrees = g_rng->RollRandomFloatInRange( 0.f, 360.f );
			asteroid->m_velocity.x = ASTEROID_SPEED * CosDegrees( driftAngleDegrees );
			asteroid->m_velocity.y = ASTEROID_SPEED * SinDegrees( driftAngleDegrees );
			return asteroid;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new asteroid, max limit reached");
	return nullptr;
}

Bullet* Game::SpawnBullet( Vec2 const& pos, float forwardDegrees )
{
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet*& bullet = m_bullets[ bulletIndex ];
		if( !bullet )
		{
			bullet = new Bullet( this, pos );
			bullet->m_orientationDegrees = forwardDegrees;
			bullet->m_velocity.x = BULLET_SPEED * CosDegrees( forwardDegrees );
			bullet->m_velocity.y = BULLET_SPEED * SinDegrees( forwardDegrees );
			return bullet;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new bullet, max limit reached");
	return nullptr;
}

Beetle* Game::SpawnNewRandomBeetle()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition();

	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle*& beetle = m_beetles[ beetleIndex ];
		if( !beetle )
		{
			beetle = new Beetle( this, spawnPosition );
			beetle->m_orientationDegrees = Atan2Degrees( m_playerShip->m_position.y - beetle->m_position.y, m_playerShip->m_position.x - beetle->m_position.x );
			beetle->m_velocity.x = BEETLE_SPEED;
			beetle->m_velocity.y = BEETLE_SPEED;
			return beetle;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new beetle, max limit reached" );
	return nullptr;
}

Wasp* Game::SpawnNewRandomWasp()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition();

	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp*& wasp = m_wasps[ waspIndex ];
		if( !wasp )
		{
			wasp = new Wasp( this, spawnPosition );
			wasp->m_orientationDegrees = Atan2Degrees( m_playerShip->m_position.y - wasp->m_position.y, m_playerShip->m_position.x - wasp->m_position.x );
			wasp->m_velocity.x = WASP_ACCELERATION;
			wasp->m_velocity.y = WASP_ACCELERATION;
			return wasp;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new wasp, max limit reached" );
	return nullptr;
}

Debris* Game::SpawnNewDebris( Vec2 const& pos, Vec2 const& vel, Rgba8 color, float scale )
{
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris*& debris = m_debris[ debrisIndex ];
		if( !debris )
		{
			debris = new Debris( this, pos, vel, color );
			debris->m_angualrVelocity = g_rng->RollRandomFloatInRange( -300.f, 300.f );
			debris->m_orientationDegrees = g_rng->RollRandomFloatInRange( 0.f, 360.f );
			TransformVertexArrayXY3D( NUM_DEBRIS_VERTS, debris->m_localVerts, scale, 0.f, Vec2( 0.f, 0.f ) );
			return debris;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn new debris, max limit reached" );
	return nullptr;
}

void Game::SpawnNewDebrisCluster( int count, Vec2 const& pos, Vec2 const& clusterVelocity, Vec2 const& forwardVector, Rgba8 color, float scale )
{
	for( int i = 0; i < count; i++ )
	{
		float heading = g_rng->RollRandomFloatInRange( -60.f, 60.f );
		float speed = g_rng->RollRandomFloatInRange( 10.f, 30.f );
		Vec2 localVelocity = Vec2::MakeFromPolarDegrees( heading, speed ) * forwardVector;
		Vec2 worldVelocity = clusterVelocity + localVelocity;
		scale *= g_rng->RollRandomFloatInRange( 0.9f, 1.1f );
		SpawnNewDebris( pos, worldVelocity, color, scale );
	}
}

Vec2 Game::GetRandomOffScreenPosition()
{
	int side = g_rng->RollRandomIntInRange( 1, 4 );

	float xPos = 0;
	float yPos = 0;
	switch( side )
	{
	case 1:
		xPos = g_rng->RollRandomFloatInRange( 0.f, WORLD_SIZE_X );
		yPos = WORLD_SIZE_Y + ASTEROID_COSMETIC_RADIUS;
		break;
	case 2:
		xPos = WORLD_SIZE_X + ASTEROID_COSMETIC_RADIUS;
		yPos = g_rng->RollRandomFloatInRange( 0.f, WORLD_SIZE_Y );
		break;
	case 3:
		xPos = g_rng->RollRandomFloatInRange( 0.f, WORLD_SIZE_X );
		yPos = 0.f - ASTEROID_COSMETIC_RADIUS;
		break;
	case 4:
		xPos = 0.f - ASTEROID_COSMETIC_RADIUS;
		yPos = g_rng->RollRandomFloatInRange( 0.f, WORLD_SIZE_Y );
		break;
	}

	return( Vec2( xPos, yPos ) );
}

void Game::UpdateEntities(float deltaSeconds)
{
	// Update Bullets
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if( bullet->IsAlive() )
		{
			bullet->Update( deltaSeconds );
		}
	}

	// Update Asteroids
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid* asteroid = m_asteroids[asteroidIndex];
		if( asteroid->IsAlive() )
		{
			asteroid->Update( deltaSeconds );
		}
	}

	// Update Beetles
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle = m_beetles[ beetleIndex ];
		if( beetle->IsAlive() )
		{
			beetle->Update( deltaSeconds, *m_playerShip );
		}
	}

	// Update Wasps
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp*& wasp = m_wasps[ waspIndex ];
		if( wasp->IsAlive() )
		{
			wasp->Update( deltaSeconds, *m_playerShip );
		}
	}

	// Update Player Ship
	if( m_playerShip )
	{
		m_playerShip->Update( deltaSeconds );
	}

	// Update Debris
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris*& debris = m_debris[ debrisIndex ];
		if( debris->IsAlive() )
		{
			debris->Update( deltaSeconds );
		}
	}
}

void Game::RenderEntities() const
{
	// Draw Bullets
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet* bullet = m_bullets[ bulletIndex ];
		if( bullet && !bullet->m_isDead )
		{
			bullet->Render();
		}
	}

	// Draw Asteroids
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid* asteroid = m_asteroids[ asteroidIndex ];
		if( asteroid && !asteroid->m_isDead )
		{
			asteroid->Render();
		}
	}

	// Render Beetles
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle = m_beetles[ beetleIndex ];
		if( beetle->IsAlive() )
		{
			beetle->Render();
		}
	}

	// Render Wasps
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp* wasp = m_wasps[ waspIndex ];
		if( wasp->IsAlive() )
		{
			wasp->Render();
		}
	}

	// Draw Player Ship
	if( m_playerShip && !m_playerShip->m_isDead )
	{
		m_playerShip->Render();
	}

	// Draw Debris
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris* debris = m_debris[ debrisIndex ];
		if( debris->IsAlive() )
		{
			debris->Render();
		}
	}
}

void Game::CheckBulletVsEnemies()
{
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid* asteroid = m_asteroids[ asteroidIndex ];

		for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
		{
			Bullet* bullet = m_bullets[ bulletIndex ];

			if( bullet && asteroid )
			{
				CheckBulletVsAsteroid( *bullet, *asteroid );
			}
		}
	}

	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle = m_beetles[ beetleIndex ];

		for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
		{
			Bullet* bullet = m_bullets[ bulletIndex ];

			if( bullet && beetle )
			{
				CheckBulletVsBeetle( *bullet, *beetle );
			}
		}
	}

	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp* wasp = m_wasps[ waspIndex ];

		for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
		{
			Bullet* bullet = m_bullets[ bulletIndex ];

			if( bullet && wasp )
			{
				CheckBulletVsWasp( *bullet, *wasp );
			}
		}
	}
}

void Game::CheckBulletVsAsteroid(Bullet& bullet, Asteroid& asteroid)
{
	if( DoEntitiesOverlap( bullet, asteroid ) )
	{
		bullet.TakeDamage( 1 );
		asteroid.TakeDamage( 1 );
	}
}

void Game::CheckBulletVsBeetle( Bullet& bullet, Beetle& beetle )
{
	if( DoEntitiesOverlap( bullet, beetle ) )
	{
		bullet.TakeDamage( 1 );
		beetle.TakeDamage( 1 );
	}
}

void Game::CheckBulletVsWasp( Bullet& bullet, Wasp& wasp )
{
	if( DoEntitiesOverlap( bullet, wasp ) )
	{
		bullet.TakeDamage( 1 );
		wasp.TakeDamage( 1 );
	}
}

void Game::CheckEnemiesVsShips()
{
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid* asteroid = m_asteroids[ asteroidIndex ];

		if( asteroid && m_playerShip->IsAlive() )
		{
			CheckAsteroidVsShip( *asteroid, *m_playerShip );
		}
	}

	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle = m_beetles[ beetleIndex ];
		
		if( beetle && m_playerShip->IsAlive() )
		{
			CheckBeetleVsShip( *beetle, *m_playerShip );
		}
	}

	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp* wasp = m_wasps[ waspIndex ];
		if( wasp && m_playerShip->IsAlive() )
		{
			CheckWaspVsShip( *wasp, *m_playerShip );
		}
	}
}

void Game::CheckAsteroidVsShip(Asteroid& asteroid, PlayerShip& ship)
{
	if( DoEntitiesOverlap( asteroid, ship ) )
	{
		asteroid.TakeDamage( 1 );
		ship.TakeDamage( 1 ); 
	}
}

void Game::CheckBeetleVsShip( Beetle& beetle, PlayerShip& ship )
{
	if( DoEntitiesOverlap( beetle, ship ) )
	{
		ship.TakeDamage( 1 );
	}
}

void Game::CheckWaspVsShip( Wasp& wasp, PlayerShip& ship )
{
	if( DoEntitiesOverlap( wasp, ship ) )
	{
		ship.TakeDamage( 1 );
	}
}

void Game::CheckEnemiesVsEnemies()
{
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle1 = m_beetles[ beetleIndex ];

		for( int secondBeetleIndex = beetleIndex + 1; secondBeetleIndex < MAX_BEETLES; secondBeetleIndex++ )
		{
			Beetle* beetle2 = m_beetles[ secondBeetleIndex ];

			if( beetle1 && beetle2 )
			{
				CheckEnemyVsEnemy( *beetle1, *beetle2 );
			}
		}
	}
}

void Game::CheckEnemyVsEnemy( Entity& enemy1, Entity& enemy2 )
{
	if( DoEntitiesOverlap( enemy1, enemy2 ) )
	{
		PushDiscsOutOfEachOther2D( enemy1.m_position, enemy1.m_cosmeticRadius, enemy2.m_position, enemy2.m_cosmeticRadius );
	}
}

void Game::CheckForGameOver()
{
	if( this == nullptr || m_playerShip == nullptr )
	{
		return;
	}

	if( m_playerShip->m_isDead && m_playerShip->m_lives <= 0 )
	{
		Debris* debris = m_debris[0];

		if( !debris->IsAlive() )
		{
			if( m_endGameTimer < 60 )
			{
				m_endGameTimer++;
			}
			else
			{
				g_app->m_isAttractMode = true;
				g_app->RestartGame();
			}
		}
	}
}

void Game::CheckIfWaveNeedsToSpawn()
{
	if( this == nullptr )
	{
		return;
	}

	bool isThereBeetle = false;
	bool isThereWasp = false;	

	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle = m_beetles[ beetleIndex ];

		if( beetle->IsAlive() )
		{
			isThereBeetle = true;
			break;
		}
	}
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp* wasp = m_wasps[ waspIndex ];

		if( wasp->IsAlive() )
		{
			isThereWasp = true;
			break;
		}
	}


	if( !isThereBeetle && !isThereWasp )
	{
		if( m_waveNumber < 5 )
		{
			SpawnWave();
		}
		else
		{
			if( m_endGameTimer < 180 )
			{
				m_endGameTimer++;
			}
			else
			{
				g_app->m_isAttractMode = true;
				g_app->RestartGame();
			}
		}
	}
}

void Game::SpawnWave()
{
	for( int i = 0; i < m_numAsteroidsPerWave[ m_waveNumber ]; i++ )
	{
		SpawnRandomAsteroid();
	}
	for( int i = 0; i < m_numBeetlesPerWave[ m_waveNumber ]; i++ )
	{
		SpawnNewRandomBeetle();
	}
	for( int i = 0; i < m_numWaspsPerWave[ m_waveNumber ]; i++ )
	{
		SpawnNewRandomWasp();
	}

	m_waveNumber++;
}


bool Game::DoEntitiesOverlap(Entity const& a, Entity const& b)
{
	float dx = b.m_position.x - a.m_position.x;
	float dy = b.m_position.y - a.m_position.y;
	float distanceSquared = ( dx * dx ) + ( dy * dy );
	float combinedRadii = a.m_physicsRadius + b.m_physicsRadius;
	float radiiSquared = combinedRadii * combinedRadii;
	return distanceSquared < radiiSquared;
}



void Game::DrawPlayerLives() const
{
	Vertex verts[ NUM_SHIP_VERTS ];
	createFakePlayerShip( verts, 127 );

	if( m_playerShip->m_lives > 1 )
	{
		TransformVertexArrayXY3D( NUM_SHIP_VERTS, verts, 1.f, 90.f, Vec2( 2.5f, 97.5f ) );
		g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, verts );
	}
	if( m_playerShip->m_lives > 2 )
	{
		TransformVertexArrayXY3D( NUM_SHIP_VERTS, verts, 1.f, 0.f, Vec2( 5.f, 0.f ) );
		g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, verts );
	}
	if( m_playerShip->m_lives > 3 )
	{
		TransformVertexArrayXY3D( NUM_SHIP_VERTS, verts, 1.f, 0.f, Vec2( 5.f, 0.f ) );
		g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, verts );
	}
}

void Game::DebugRenderEntities() const
{
	// Debug Draw Bullets
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if( bullet->IsAlive() )
		{
			bullet->DebugRender();
		}
	}

	// Debug Draw Asteroids
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid* asteroid = m_asteroids[asteroidIndex];
		if( asteroid->IsAlive() )
		{
			asteroid->DebugRender();
		}
	}

	// Debug Draw Beetles
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle = m_beetles[ beetleIndex ];
		if( beetle->IsAlive() )
		{
			beetle->DebugRender();
		}
	}

	// Debug Draw Wasps
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp* wasp = m_wasps[ waspIndex ];
		if( wasp->IsAlive() )
		{
			wasp->DebugRender();
		}
	}

	// Debug Draw Player Ship
	if( m_playerShip )
	{
		m_playerShip->DebugRender();
	}

	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris* debris = m_debris[ debrisIndex ];
		if( debris->IsAlive() )
		{
			debris->DebugRender();
		}
	}
}

void Game::DeleteGarbageEntities()
{
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid*& asteroid = m_asteroids[ asteroidIndex ];
		if( asteroid && asteroid->m_isGarbage )
		{
			delete asteroid;
			asteroid = nullptr;
		}
	}

	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet*& bullet = m_bullets[ bulletIndex ];
		if( bullet && bullet->m_isGarbage )
		{
			delete bullet;
			bullet = nullptr;
		}
	}

	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle*& beetle = m_beetles[ beetleIndex ];
		if( beetle && beetle->m_isGarbage )
		{
			delete beetle;
			beetle = nullptr;
		}
	}

	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp*& wasp = m_wasps[ waspIndex ];
		if( wasp && wasp->m_isGarbage )
		{
			delete wasp;
			wasp = nullptr;
		}
	}

	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris*& debris = m_debris[ debrisIndex ];
		if( debris && debris->m_isGarbage )
		{
			delete debris;
			debris = nullptr;
		}
	}
}


/*
Update Cameras
{
	worldcamera.setOrthoview( Vec2(0.f o.f) Vec2(WorldSizeXm WORLDSIZEY));
	screenCamera.setOrthoview( Vec3(o.f, 0.f), Vec2( ScreenSIZEX, SCREENSIZEY));

	float maxCameraShake = (m_shakeamt * m_screenShaketrauma
	float shake x = g.rngRollrandomFloatInRange( -m_shakeamt, m_shakeamt )
	float shake y = g.rngRollrandomFloatInRange( -m_shakeamt, m_shakeamt )

	camera.translate2D( Vec2 ( shakex, shakey ));

	constexpr float MAXSSCREENSHAKE = 10.f
	constexptrfloat SXREENSHAKEREDUCTION = 2.f;
	m_screenshaketrauma -= SCREEN_SHAKE_REDUCTION * deltaseconds
	m_screenshaketrauma = clamp( m_screenshale, 0.f, MAXSREENSHAKE
}
*/