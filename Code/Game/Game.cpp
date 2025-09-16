#include "Game/Game.hpp"
#include "PlayerShip.hpp"
#include "Game/App.hpp"
#include "Bullet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Asteroid.hpp"

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

	for (int i = 0; i < NUM_STARTING_ASTEROIDS; ++i)
	{
		SpawnRandomAsteroid();
	}
}

void Game::Update(float deltaSeconds)
{
	UpdateEntities( deltaSeconds );
	CheckBulletsVsAsteroids();
	CheckAsteroidsVsShips();
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
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid*& asteroid = m_asteroids[ asteroidIndex ];
		if( asteroid )
		{
			delete asteroid;
			asteroid = nullptr;
		}
	}

	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet*& bullet = m_bullets[ bulletIndex ];
		if( bullet )
		{
			delete bullet;
			bullet = nullptr;
		}
	}
	
	if( m_playerShip )
	{
		delete m_playerShip;
		m_playerShip = nullptr;
	}
}

Asteroid* Game::SpawnRandomAsteroid()
{
	RandomNumberGenerator rng;
	float x = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_X );
	float y = rng.RollRandomFloatInRange( 0.f, WORLD_SIZE_Y );

	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid*& asteroid = m_asteroids[ asteroidIndex ];
		if( !asteroid )
		{
			asteroid = new Asteroid( this, Vec2( x, y ));
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

void Game::UpdateEntities(float deltaSeconds)
{
	// Update Bullets
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if( bullet && !bullet->m_isDead )
		{
			bullet->Update( deltaSeconds );
		}
	}

	// Update Asteroids
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid* asteroid = m_asteroids[asteroidIndex];
		if( asteroid && !asteroid->m_isDead )
		{
			asteroid->Update( deltaSeconds );
		}
	}

	// Update Player Ship
	if( m_playerShip && !m_playerShip->m_isDead )
	{
		m_playerShip->Update( deltaSeconds );
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

	// Draw Player Ship
	if( m_playerShip && !m_playerShip->m_isDead)
	{
		m_playerShip->Render();
	}
}

void Game::CheckBulletsVsAsteroids()
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
}

void Game::CheckBulletVsAsteroid(Bullet& bullet, Asteroid& asteroid)
{
	if( DoEntitiesOverlap( bullet, asteroid ) )
	{
		bullet.TakeDamage( 1 );
		asteroid.TakeDamage( 1 );
	}
}

void Game::CheckAsteroidsVsShips()
{
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid* asteroid = m_asteroids[ asteroidIndex ];

		if( asteroid && !m_playerShip->m_isDead )
		{
		CheckAsteroidVsShip( *asteroid, *m_playerShip );
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
	//DebugDrawRing( Vec2( 50.f, 20.f ), 10.f, 2.f, Rgba8( 255, 100, 0) );
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

	// Debug Draw Player Ship
	if( m_playerShip && !m_playerShip->m_isDead )
	{
		m_playerShip->DebugRender();
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
}

