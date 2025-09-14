#include "Game/Game.hpp"
#include "PlayerShip.hpp"

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
		//SpawnRandomAsteroid();
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

	/*if (g_debugDraw)
	{
		DebugRenderEntities();
	}*/
}

void Game::Shutdown()
{
	//delete m_playerShip;
	//m_playerShip = nullptr
}

//Asteroid* Game::SpawnRandomAsteroid()
//{
//	
//}
//
//Bullet* Game::SpawnBullet( Vec2 const& pos, float forwardDegrees )
//{
//
//}

void Game::UpdateEntities(float deltaSeconds)
{
	m_playerShip->Update( deltaSeconds );
}

void Game::RenderEntities() const
{
	// Draw Bullets


	// Draw Asteroids


	// Draw Player Ship
	m_playerShip->Render();
}

void Game::CheckBulletsVsAsteroids()
{

}

void Game::CheckBulletVsAsteroid(Bullet& bullet, Asteroid& asteroid)
{

}

void Game::CheckAsteroidsVsShips()
{

}

void Game::CheckAsteroidVsShip(Asteroid& asteroid, PlayerShip& ship)
{

}

void Game::DoEntitiesOverlap(Entity const& a, Entity const& b)
{

}

void Game::DebugRenderEntities() const
{

}

void Game::DeleteGarbageEntities()
{

}

