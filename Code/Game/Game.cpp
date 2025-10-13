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
#include "Game/FireBullet.hpp"
#include "Game/IceBullet.hpp"
#include "Game/FieryAsteroid.hpp"
#include "Game/IcyAsteroid.hpp"
#include "Game/Explosion.hpp"
#include "Game/FireBeetle.hpp"
#include "Game/IceBeetle.hpp"
#include "Game/FireWasp.hpp"
#include "Game/IceWasp.hpp"

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

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_VIEW_SIZE_X, WORLD_VIEW_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
	
	CreateStarfield();

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
	UpdateCameras( deltaSeconds );

}

void Game::Render() const
{
	RenderStars();

	RenderEntities();
	DrawPlayerLives();

	if ( m_app->m_debugDraw )
	{
		DebugRenderEntities();
		DebugDrawWorldBounds();
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

	for( int explosionIndex = 0; explosionIndex < MAX_EXPLOSIONS; explosionIndex++ )
	{
		Explosion*& explosion = m_explosions[ explosionIndex ];
		if( explosion )
		{
			delete explosion;
			explosion = nullptr;
		}
	}
}

Asteroid* Game::SpawnRandomFieryAsteroid()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition();

	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid*& asteroid = m_asteroids[ asteroidIndex ];
		if( !asteroid )
		{
			asteroid = new FieryAsteroid( this, spawnPosition );
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

Asteroid* Game::SpawnRandomIcyAsteroid()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition();

	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid*& asteroid = m_asteroids[ asteroidIndex ];
		if( !asteroid )
		{
			asteroid = new IcyAsteroid( this, spawnPosition );
			asteroid->m_angualrVelocity = g_rng->RollRandomFloatInRange( -300.f, 300.f );
			asteroid->m_orientationDegrees = g_rng->RollRandomFloatInRange( 0.f, 360.f );
			float driftAngleDegrees = g_rng->RollRandomFloatInRange( 0.f, 360.f );
			asteroid->m_velocity.x = ASTEROID_SPEED * CosDegrees( driftAngleDegrees );
			asteroid->m_velocity.y = ASTEROID_SPEED * SinDegrees( driftAngleDegrees );

			return asteroid;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new asteroid, max limit reached" );
	return nullptr;
}

Bullet* Game::SpawnFireBullet( Vec2 const& pos, float forwardDegrees )
{
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet*& bullet = m_bullets[ bulletIndex ];
		if( !bullet )
		{
			float bulletOffset = g_rng->RollRandomFloatInRange( -5.f, 5.f );
			Vec2 bulletOffsetVector = Vec2( bulletOffset, bulletOffset );

			bullet = new FireBullet( this, pos );
			bullet->m_orientationDegrees = forwardDegrees;
			bullet->m_velocity.x = BULLET_SPEED * CosDegrees( forwardDegrees + bulletOffset );
			bullet->m_velocity.y = BULLET_SPEED * SinDegrees( forwardDegrees + bulletOffset );

			return bullet;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new bullet, max limit reached");
	return nullptr;
}

Bullet* Game::SpawnIceBullet( Vec2 const& pos, float forwardDegrees )
{
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet*& bullet = m_bullets[ bulletIndex ];
		if( !bullet )
		{
			float bulletOffset = g_rng->RollRandomFloatInRange( -5.f, 5.f );
			Vec2 bulletOffsetVector = Vec2( bulletOffset, bulletOffset );

			bullet = new IceBullet( this, pos );
			bullet->m_orientationDegrees = forwardDegrees;
			bullet->m_velocity.x = BULLET_SPEED * CosDegrees( forwardDegrees + bulletOffset );
			bullet->m_velocity.y = BULLET_SPEED * SinDegrees( forwardDegrees + bulletOffset );

			return bullet;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new bullet, max limit reached" );
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
			
			return beetle;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new beetle, max limit reached" );
	return nullptr;
}

Beetle* Game::SpawnNewRandomFireBeetle()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition();

	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle*& beetle = m_beetles[ beetleIndex ];
		if( !beetle )
		{
			beetle = new FireBeetle( this, spawnPosition );
			beetle->m_orientationDegrees = Atan2Degrees( m_playerShip->m_position.y - beetle->m_position.y, m_playerShip->m_position.x - beetle->m_position.x );

			return beetle;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new beetle, max limit reached" );
	return nullptr;
}

Beetle* Game::SpawnNewRandomIceBeetle()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition();

	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle*& beetle = m_beetles[ beetleIndex ];
		if( !beetle )
		{
			beetle = new IceBeetle( this, spawnPosition );
			beetle->m_orientationDegrees = Atan2Degrees( m_playerShip->m_position.y - beetle->m_position.y, m_playerShip->m_position.x - beetle->m_position.x );

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
			
			return wasp;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new wasp, max limit reached" );
	return nullptr;
}

Wasp* Game::SpawnNewRandomFireWasp()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition();

	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp*& wasp = m_wasps[ waspIndex ];
		if( !wasp )
		{
			wasp = new FireWasp( this, spawnPosition );
			wasp->m_orientationDegrees = Atan2Degrees( m_playerShip->m_position.y - wasp->m_position.y, m_playerShip->m_position.x - wasp->m_position.x );

			return wasp;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new wasp, max limit reached" );
	return nullptr;
}

Wasp* Game::SpawnNewRandomIceWasp()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition();

	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp*& wasp = m_wasps[ waspIndex ];
		if( !wasp )
		{
			wasp = new IceWasp( this, spawnPosition );
			wasp->m_orientationDegrees = Atan2Degrees( m_playerShip->m_position.y - wasp->m_position.y, m_playerShip->m_position.x - wasp->m_position.x );

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
		float heading = g_rng->RollRandomFloatInRange( 0.f, 360.f );
		float speed = g_rng->RollRandomFloatInRange( 10.f, 30.f );
		Vec2 localVelocity = Vec2::MakeFromPolarDegrees( heading, speed ) * forwardVector;
		Vec2 worldVelocity = clusterVelocity + localVelocity;
		scale *= g_rng->RollRandomFloatInRange( 0.9f, 1.1f );
		SpawnNewDebris( pos, worldVelocity, color, scale );
	}
}

Explosion* Game::SpawnNewExplosion( Vec2 const& pos, Rgba8 color )
{
	for( int explosionIndex = 0; explosionIndex < MAX_EXPLOSIONS; explosionIndex++ )
	{
		Explosion*& explosion = m_explosions[ explosionIndex ];
		if( !explosion )
		{
			explosion = new Explosion( this, pos, color );
			return explosion;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn new explosion, max limit reached" );
	return nullptr;
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

	// Update Explosions
	for( int explosionIndex = 0; explosionIndex < MAX_EXPLOSIONS; explosionIndex++ )
	{
		Explosion*& explosion = m_explosions[ explosionIndex ];
		if( explosion->IsAlive() )
		{
			explosion->Update( deltaSeconds );
		}
	}
}

void Game::UpdateCameras( float deltaSeconds )
{
	Vec2 minView( m_playerShip->m_position.x - WORLD_VIEW_SIZE_X * 0.5f, m_playerShip->m_position.y - WORLD_VIEW_SIZE_Y * 0.5f );
	Vec2 maxView( m_playerShip->m_position.x + WORLD_VIEW_SIZE_X * 0.5f, m_playerShip->m_position.y + WORLD_VIEW_SIZE_Y * 0.5f );
	Vec2 worldViewSize( WORLD_VIEW_SIZE_X, WORLD_VIEW_SIZE_Y );

	ClampCamera( minView, maxView );

	m_worldCamera->SetOrthoView( minView, maxView );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	if( !m_isShaking )
	{
		return;
	}

	float shakeX = g_rng->RollRandomFloatInRange( -m_screenShakeAmount, m_screenShakeAmount );
	float shakeY = g_rng->RollRandomFloatInRange( -m_screenShakeAmount, m_screenShakeAmount );

	m_worldCamera->Translate2D( Vec2( shakeX, shakeY ) );

	m_screenShakeAmount -= SCREEN_SHAKE_REDUCTION * deltaSeconds;
	m_screenShakeAmount = GetClamped( m_screenShakeAmount, 0.f, MAX_SCREEN_SHAKE_AMOUNT );

	if( m_screenShakeAmount <= 0.f )
	{
		m_isShaking = false;
		m_screenShakeAmount = MAX_SCREEN_SHAKE_AMOUNT;
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

	// Draw Beetles
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle = m_beetles[ beetleIndex ];
		if( beetle->IsAlive() )
		{
			beetle->Render();
		}
	}

	// Draw Wasps
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

	// Draw Explosions
	for( int explosionIndex = 0; explosionIndex < MAX_EXPLOSIONS; explosionIndex++ )
	{
		Explosion* explosion = m_explosions[ explosionIndex ];
		if( explosion->IsAlive() )
		{
			explosion->Render();
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
		
		if( bullet.m_isFireBullet )
		{
			if( beetle.m_isOnFire )
			{
				beetle.ResetFireTick();
			}
			else
			{
				beetle.m_isOnFire = true;
			}
		}
		else
		{
			if( beetle.m_isSlow )
			{
				beetle.ResetSlowTimer();
			}
			else
			{
				beetle.m_isSlow = true;
			}
		}
	}
}

void Game::CheckBulletVsWasp( Bullet& bullet, Wasp& wasp )
{
	if( DoEntitiesOverlap( bullet, wasp ) )
	{
		bullet.TakeDamage( 1 );
		wasp.TakeDamage( 1 );

		if( bullet.m_isFireBullet )
		{
			if( wasp.m_isOnFire )
			{
				wasp.ResetFireTick();
			}
			else
			{
				wasp.m_isOnFire = true;
			}
		}
		else
		{
			if( wasp.m_isSlow )
			{
				wasp.ResetSlowTimer();
			}
			else
			{
				wasp.m_isSlow = true;
			}
		}
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

void Game::ClampCamera( Vec2& minView, Vec2& maxView )
{
	if( minView.x < 0.f )
	{
		minView.x = 0.f;
		maxView.x = WORLD_VIEW_SIZE_X;
	}
	else if( maxView.x > WORLD_SIZE_X )
	{
		minView.x = WORLD_SIZE_X - WORLD_VIEW_SIZE_X;
		maxView.x = WORLD_SIZE_X;
	}
	
	if( minView.y < 0.f )
	{
		minView.y = 0.f;
		maxView.y = WORLD_VIEW_SIZE_Y;
	}
	else if( maxView.y > WORLD_SIZE_Y )
	{
		minView.y = WORLD_SIZE_Y - WORLD_VIEW_SIZE_Y;
		maxView.y = WORLD_SIZE_Y;
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
				g_app->m_nextState = GAME_STATE_ATTRACT;
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
		if( m_waveNumber < NUM_WAVES )
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
				g_app->m_nextState = GAME_STATE_ATTRACT;
				g_app->RestartGame();
			}
		}
	}
}

void Game::SpawnWave()
{
	for( int i = 0; i < m_numFieryAsteroidsPerWave[ m_waveNumber ]; i++ )
	{
		SpawnRandomFieryAsteroid();
	}
	for( int i = 0; i < m_numIcyAsteroidsPerWave[ m_waveNumber ]; i++ )
	{
		SpawnRandomIcyAsteroid();
	}

	for( int i = 0; i < m_numBeetlesPerWave[ m_waveNumber ]; i++ )
	{
		SpawnNewRandomBeetle();
	}
	for( int i = 0; i < m_numFireBeetlesPerWave[ m_waveNumber ]; i++ )
	{
		SpawnNewRandomFireBeetle();
	}
	for( int i = 0; i < m_numIceBeetlesPerWave[ m_waveNumber ]; i++ )
	{
		SpawnNewRandomIceBeetle();
	}

	for( int i = 0; i < m_numWaspsPerWave[ m_waveNumber ]; i++ )
	{
		SpawnNewRandomWasp();
	}
	for( int i = 0; i < m_numFireWaspsPerWave[ m_waveNumber ]; i++ )
	{
		SpawnNewRandomFireWasp();
	}
	for( int i = 0; i < m_numIceWaspsPerWave[ m_waveNumber ]; i++ )
	{
		SpawnNewRandomIceWasp();
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
	g_engine->m_render->EndCamera( *m_worldCamera );
	g_engine->m_render->BeginCamera( *m_screenCamera );

	Vertex verts[ NUM_SHIP_VERTS ];
	createFakePlayerShip( verts, 127 );

	if( m_playerShip->m_lives > 1 )
	{
		TransformVertexArrayXY3D( NUM_SHIP_VERTS, verts, 8.f, 90.f, Vec2( 20.f, 780.f ) );
		g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, verts );
	}
	if( m_playerShip->m_lives > 2 )
	{
		TransformVertexArrayXY3D( NUM_SHIP_VERTS, verts, 1.f, 0.f, Vec2( 40.f, 0.f ) );
		g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, verts );
	}
	if( m_playerShip->m_lives > 3 )
	{
		TransformVertexArrayXY3D( NUM_SHIP_VERTS, verts, 1.f, 0.f, Vec2( 40.f, 0.f ) );
		g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, verts );
	}

	g_engine->m_render->EndCamera( *m_screenCamera );
	g_engine->m_render->BeginCamera( *m_worldCamera );
}

void Game::CreateStarfield()
{
	for( int starNumber = 0; starNumber < MAX_STARS; starNumber++ )
	{
		Vertex& vert1Near = m_starVertsNear[ 6 * starNumber ];
		Vertex& vert2Near = m_starVertsNear[ 6 * starNumber + 1 ];
		Vertex& vert3Near = m_starVertsNear[ 6 * starNumber + 2 ];
		Vertex& vert4Near = m_starVertsNear[ 6 * starNumber + 3 ];
		Vertex& vert5Near = m_starVertsNear[ 6 * starNumber + 4 ];
		Vertex& vert6Near = m_starVertsNear[ 6 * starNumber + 5 ];

		Vertex& vert1Far = m_starVertsFar[ 6 * starNumber ];
		Vertex& vert2Far = m_starVertsFar[ 6 * starNumber + 1 ];
		Vertex& vert3Far = m_starVertsFar[ 6 * starNumber + 2 ];
		Vertex& vert4Far = m_starVertsFar[ 6 * starNumber + 3 ];
		Vertex& vert5Far = m_starVertsFar[ 6 * starNumber + 4 ];
		Vertex& vert6Far = m_starVertsFar[ 6 * starNumber + 5 ];

		float starX = g_rng->RollRandomFloatInRange( -WORLD_VIEW_SIZE_X, WORLD_SIZE_X + WORLD_VIEW_SIZE_X );
		float starY = g_rng->RollRandomFloatInRange( -WORLD_VIEW_SIZE_Y, WORLD_SIZE_Y + WORLD_VIEW_SIZE_Y );

		vert1Near.m_pos = Vec3( starX + 0.5, starY, -0.f );
		vert2Near.m_pos = Vec3( starX - 0.5, starY, 0.f );
		vert3Near.m_pos = Vec3( starX, starY + 0.5, 0.f );
		vert4Near.m_pos = Vec3( starX + 0.5, starY, 0.f );
		vert5Near.m_pos = Vec3( starX - 0.5, starY, 0.f );
		vert6Near.m_pos = Vec3( starX, starY - 0.5, 0.f );

		vert1Far.m_pos = Vec3( starX + 0.5, starY, -0.f );
		vert2Far.m_pos = Vec3( starX - 0.5, starY, 0.f );
		vert3Far.m_pos = Vec3( starX, starY + 0.5, 0.f );
		vert4Far.m_pos = Vec3( starX + 0.5, starY, 0.f );
		vert5Far.m_pos = Vec3( starX - 0.5, starY, 0.f );
		vert6Far.m_pos = Vec3( starX, starY - 0.5, 0.f );
	}
}

void Game::RenderStars() const
{
	Vec2 displacementCenterToPlayer = Vec2();
	if( m_playerShip )
	{
		displacementCenterToPlayer = m_playerShip->m_position - Vec2( WORLD_CENTER_X, WORLD_CENTER_Y);
	}

	Vertex tempStarVerts[ NUM_STAR_VERTS ];
	
	Vec2 farStarDisplacement = -0.2f * displacementCenterToPlayer + Vec2( 5.f, 6.f );
	for( int starIndex = 0; starIndex < NUM_STAR_VERTS; starIndex++ )
	{
		Vertex& vert = tempStarVerts[ starIndex ];
		vert = m_starVertsFar[ starIndex ];
		vert.m_pos.x += farStarDisplacement.x;
		vert.m_pos.y += farStarDisplacement.y;
		vert.m_color = Rgba8( 100, 100, 100 );
	}
	g_engine->m_render->DrawVertexArray( NUM_STAR_VERTS, tempStarVerts );

	Vec2 nearStarDisplacement = -0.5f * displacementCenterToPlayer;
	for( int starIndex = 0; starIndex < NUM_STAR_VERTS; starIndex++ )
	{
		Vertex& vert = tempStarVerts[ starIndex ];
		vert = m_starVertsNear[ starIndex ];
		vert.m_pos.x += nearStarDisplacement.x;
		vert.m_pos.y += nearStarDisplacement.y;
		vert.m_color = Rgba8( 255, 255, 255 );
	}
	g_engine->m_render->DrawVertexArray( NUM_STAR_VERTS, tempStarVerts );
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

	// Debug Draw Debris
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris* debris = m_debris[ debrisIndex ];
		if( debris->IsAlive() )
		{
			debris->DebugRender();
		}
	}

	// Debug Draw Explosions
	for( int explosionIndex = 0; explosionIndex < MAX_EXPLOSIONS; explosionIndex++ )
	{
		Explosion* explosion = m_explosions[ explosionIndex ];
		if( explosion->IsAlive() )
		{
			explosion->DebugRender();
		}
	}
}

void Game::DebugDrawWorldBounds() const
{
	DebugDrawLine( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, WORLD_SIZE_Y ), Vec2( WORLD_SIZE_X, 0.f ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, 0.f ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, WORLD_SIZE_Y ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, 0.f ), Vec2( 0.f, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( WORLD_SIZE_X, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
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

	for( int explosionIndex = 0; explosionIndex < MAX_EXPLOSIONS; explosionIndex++ )
	{
		Explosion*& explosion = m_explosions[ explosionIndex ];
		if( explosion && explosion->m_isGarbage )
		{
			delete explosion;
			explosion = nullptr;
		}
	}
}