#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/App.hpp"
#include "Game/Bullet.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Beetle.hpp"
#include "Game/Wasp.hpp"
#include "Game/Debris.hpp"

RandomNumberGenerator rng;

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
	Vec2 worldCenter( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f );
	m_playerShip = new PlayerShip( this, worldCenter );

	for( int i = 0; i < NUM_STARTING_ASTEROIDS; i++ )
	{
		SpawnRandomAsteroid();
	}
	for( int i = 0; i < 1; i++ )
	{
		SpawnNewRandomBeetle();
		SpawnNewRandomWasp();
	}
}

void Game::Update(float deltaSeconds)
{

	UpdateEntities( deltaSeconds );
	CheckBulletVsEnemies();
	CheckEnemiesVsShips();
	DeleteGarbageEntities();

}

void Game::Render() const
{
	RenderEntities();

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
	float x = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_X );
	float y = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_Y );

	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid*& asteroid = m_asteroids[ asteroidIndex ];
		if( !asteroid )
		{
			asteroid = new Asteroid( this, Vec2( x, y ) );
			asteroid->m_angualrVelocity = rng.RollRandomFloatInRange( -300.f, 300.f );
			asteroid->m_orientationDegrees = rng.RollRandomFloatInRange( 0.f, 360.f );
			float driftAngleDegrees = rng.RollRandomFloatInRange( 0.f, 360.f );
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
	float x = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_X );
	float y = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_Y );

	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle*& beetle = m_beetles[ beetleIndex ];
		if( !beetle )
		{
			beetle = new Beetle( this, Vec2( x, y ) );
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
	float x = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_X );
	float y = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_Y );

	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp*& wasp = m_wasps[ waspIndex ];
		if( !wasp )
		{
			wasp = new Wasp( this, Vec2( x, y ) );
			wasp->m_orientationDegrees = Atan2Degrees( m_playerShip->m_position.y - wasp->m_position.y, m_playerShip->m_position.x - wasp->m_position.x );
			wasp->m_velocity.x = WASP_ACCELERATION;
			wasp->m_velocity.y = WASP_ACCELERATION;
			return wasp;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new wasp, max limit reached" );
	return nullptr;
}

Debris* Game::SpawnNewDebris( Vec2 const& pos, Vec2 const& vel, Rgba8 color )
{
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris*& debris = m_debris[ debrisIndex ];
		if( !debris )
		{
			debris = new Debris( this, pos, vel, color );
			debris->m_angualrVelocity = rng.RollRandomFloatInRange( -300.f, 300.f );
			debris->m_orientationDegrees = rng.RollRandomFloatInRange( 0.f, 360.f );
			return debris;
		}
	}
}

void Game::SpawnNewDebrisCluster( int count, Vec2 const& pos, Vec2 const& clusterVelocity, float orientation, Rgba8 color )
{
	for( int i = 0; i < count; i++ )
	{
		float heading = rng.RollRandomFloatInRange( -45.f, 45.f ) + orientation;
		float speed = rng.RollRandomFloatInRange( 10.f, 20.f );
		Vec2 localVelocity = Vec2::MakeFromPolarDegrees( heading, speed );
		Vec2 worldVelocity = clusterVelocity + localVelocity;
		SpawnNewDebris( pos, worldVelocity, color );
	}
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

bool Game::DoEntitiesOverlap(Entity const& a, Entity const& b)
{
	float dx = b.m_position.x - a.m_position.x;
	float dy = b.m_position.y - a.m_position.y;
	float distanceSquared = ( dx * dx ) + ( dy * dy );
	float combinedRadii = a.m_physicsRadius + b.m_physicsRadius;
	float radiiSquared = combinedRadii * combinedRadii;
	return distanceSquared < radiiSquared;
}

void Game::DebugRenderEntities() const
{
	// Debug Draw Bullets
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if( bullet && !bullet->m_isDead )
		{
			bullet->DebugRender();
		}
	}

	// Debug Draw Asteroids
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid* asteroid = m_asteroids[asteroidIndex];
		if( asteroid && !asteroid->m_isDead )
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

/*Debris* Game::SpawnNewDebris( Vec2 const& pos, Vec2 const& vel, float radius, Rgba8 color, float lifetimeSeconds )
{
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris*& debris = m_debris[debrisIndex];
		if( debris == nullptr )
		{
			debris = new Debris( this, pos, vel, radius, color, lifetimeSeconds);
			return;
		}
	}
}*/
