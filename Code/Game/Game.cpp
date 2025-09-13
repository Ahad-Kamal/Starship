#include "Game/Game.hpp"
#include "PlayerShip.hpp"

Game::Game(App* owner)
{
	
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

}

void Game::UpdateEntities(float deltaSeconds)
{

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

void Game::RenderEntities() const
{
	// Draw Bullets


	// Draw Asteroids


	// Draw Player Ship
}

void Game::DebugRenderEntities() const
{

}

void Game::DeleteGarbageEntities()
{

}

