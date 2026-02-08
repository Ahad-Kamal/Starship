#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
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


//-----------------------------------------------------------------------------------------------
RandomNumberGenerator* g_rng = nullptr;

//-----------------------------------------------------------------------------------------------
Game::Game( App* owner )
{
	m_app = owner;
	Startup();
}

//-----------------------------------------------------------------------------------------------
Game::~Game()
{
	
}

//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
	m_worldCamera = new Camera();
	m_screenCamera = new Camera();

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_VIEW_SIZE_X, WORLD_VIEW_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	InitializeStartTriangleVerts();
	TransformVertexArrayXY3D( 3, m_startVerts, 1.f, 0.f, Vec2( SCREEN_CENTER_X, SCREEN_CENTER_Y ) );
	CreateFakePlayerShip( m_fakeShipVerts, 255 );

	CreateStarfield();

	Vec2 worldCenter( WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f );
	m_playerShip = new PlayerShip( this, worldCenter );

	SpawnWave();
}

//-----------------------------------------------------------------------------------------------
void Game::Update( float deltaSeconds )
{
	UpdateStates();
	UpdateKeyboardInput();
	UpdateControllerInput();

	if( m_currentState == GAME_STATE_ATTRACT )
	{
		UpdateAttractMode( deltaSeconds );
		return;
	}

	UpdateEntities( deltaSeconds );

	CheckBulletVsEnemies();
	CheckShipsVsEnemies();
	CheckEnemiesVsEnemies();
	CheckExplosionsVsEnemies();

	DeleteGarbageEntities();
	CheckIfWaveNeedsToSpawn();
	CheckForGameOver();

	UpdateCameras( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	if( m_currentState == GAME_STATE_ATTRACT )
	{
		RenderAttractMode();
		return;
	}

	RenderStars();
	RenderEntities();
	DrawPlayerLives();

	if ( m_debugDraw )
	{
		DebugRenderEntities();
		DebugDrawWorldBounds();
	}
}

//-----------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	// Delete Bullets
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet*& bullet = m_bullets[ bulletIndex ];
		if( bullet )
		{
			delete bullet;
			bullet = nullptr;
		}
	}

	// Delete Asteroids
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid*& asteroid = m_asteroids[ asteroidIndex ];
		if( asteroid )
		{
			delete asteroid;
			asteroid = nullptr;
		}
	}

	// Delete Beetles
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle*& beetle = m_beetles[ beetleIndex ];
		if( beetle  )
		{
			delete beetle;
			beetle = nullptr;
		}
	}

	// Delete Wasps
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp*& wasp = m_wasps[ waspIndex ];
		if( wasp )
		{
			delete wasp;
			wasp = nullptr;
		}
	}
	
	// Delete Player Ship
	if( m_playerShip )
	{
		delete m_playerShip;
		m_playerShip = nullptr;
	}

	// Delete Debris
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris*& debris = m_debris[ debrisIndex ];
		if( debris )
		{
			delete debris;
			debris = nullptr;
		}
	}

	// Delete Explosions
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

//-----------------------------------------------------------------------------------------------
Asteroid* Game::SpawnNewRandomFieryAsteroid()
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

//-----------------------------------------------------------------------------------------------
Asteroid* Game::SpawnNewRandomIcyAsteroid()
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

//-----------------------------------------------------------------------------------------------
Bullet* Game::SpawnNewFireBullet( Vec2 const& pos, float forwardDegrees )
{
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet*& bullet = m_bullets[ bulletIndex ];
		if( !bullet )
		{
			float bulletOffset = g_rng->RollRandomFloatInRange( -10.f, 10.f );
			Vec2 bulletOffsetVector = Vec2( bulletOffset, bulletOffset );

			bullet = new FireBullet( this, pos );
			bullet->m_orientationDegrees = forwardDegrees;
			bullet->m_velocity.x = BULLET_SPEED * CosDegrees( forwardDegrees + bulletOffset );
			bullet->m_velocity.y = BULLET_SPEED * SinDegrees( forwardDegrees + bulletOffset );

			g_engine->m_audio->StartSound( audio_fireShoot, false, 0.05f );
			return bullet;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new bullet, max limit reached");
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
Bullet* Game::SpawnNewIceBullet( Vec2 const& pos, float forwardDegrees )
{
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet*& bullet = m_bullets[ bulletIndex ];
		if( !bullet )
		{
			float bulletOffset = g_rng->RollRandomFloatInRange( -10.f, 10.f );
			Vec2 bulletOffsetVector = Vec2( bulletOffset, bulletOffset );

			bullet = new IceBullet( this, pos );
			bullet->m_orientationDegrees = forwardDegrees;
			bullet->m_velocity.x = BULLET_SPEED * CosDegrees( forwardDegrees + bulletOffset );
			bullet->m_velocity.y = BULLET_SPEED * SinDegrees( forwardDegrees + bulletOffset );

			g_engine->m_audio->StartSound( audio_iceShoot, false, 0.05f );
			return bullet;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn a new bullet, max limit reached" );
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
void Game::SpawnNewDebrisCluster( int count, Vec2 const& pos, Vec2 const& clusterVelocity, [[maybe_unused]] Vec2 const& forwardVector, Rgba8 color, float scale )
{
	for( int i = 0; i < count; i++ )
	{
		float heading = g_rng->RollRandomFloatInRange( 0.f, 360.f );
		float speed = g_rng->RollRandomFloatInRange( 10.f, 30.f );
		Vec2 localVelocity = Vec2::MakeFromPolarDegrees( heading, speed );
		Vec2 worldVelocity = clusterVelocity + localVelocity;
		scale *= g_rng->RollRandomFloatInRange( 0.9f, 1.1f );
		SpawnNewDebris( pos, worldVelocity, color, scale );
	}
}

//-----------------------------------------------------------------------------------------------
Explosion* Game::SpawnNewExplosion( Vec2 const& pos, Rgba8 color, bool explosionType ) // true if fiery, false if icy
{
	for( int explosionIndex = 0; explosionIndex < MAX_EXPLOSIONS; explosionIndex++ )
	{
		Explosion*& explosion = m_explosions[ explosionIndex ];
		if( !explosion )
		{
			explosion = new Explosion( this, pos, color, explosionType );

			if( explosionType )
			{
				g_engine->m_audio->StartSound( audio_fireExplosion, false, 0.1f );
			}
			else
			{
				g_engine->m_audio->StartSound( audio_iceExplosion, false, 0.1f );
			}

			return explosion;
		}
	}

	ERROR_RECOVERABLE( "Can't spawn new explosion, max limit reached" );
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
Vec2 Game::GetRandomOffScreenPosition() const
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

//-----------------------------------------------------------------------------------------------
void Game::AddCameraShake( float shakeAmount )
{
	m_isShaking = true;
	m_screenShakeAmount += shakeAmount;
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode( [[maybe_unused]] float deltaSeconds )
{
	m_shipRotation += 0.5f;
	if( m_shipRotation >= 360.f )
	{
		m_shipRotation = fmodf( m_shipRotation, 360.f );
	}

	m_time += deltaSeconds * 1.25f;
	m_startAlpha = 127.5f * cosf( m_time * 2.0f ) + 127.5f;

	for( int startIndex = 0; startIndex < 3; startIndex++ )
	{
		m_startVerts[ startIndex ].m_color = Rgba8( 0, 255, 0, (unsigned int)m_startAlpha );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	g_engine->m_render->BeginCamera( *m_screenCamera );

	g_engine->m_render->ClearScreen( m_clearColor );

	// Draw Ships
	Vertex tempShipVerts[ NUM_SHIP_VERTS ];
	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; vertIndex++ )
	{
		tempShipVerts[ vertIndex ] = m_fakeShipVerts[ vertIndex ];
	}

	TransformVertexArrayXY3D( NUM_SHIP_VERTS, tempShipVerts, 80.f, m_shipRotation, Vec2( 300.f, 400.f ) );
	g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, tempShipVerts );

	TransformVertexArrayXY3D( NUM_SHIP_VERTS, tempShipVerts, 1.f, 0.f, Vec2( -300.f, -400.f ) );
	TransformVertexArrayXY3D( NUM_SHIP_VERTS, tempShipVerts, 1.f, 180.f, Vec2( 1300.f, 400.f ) );
	g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, tempShipVerts );

	// Draw Start Button
	g_engine->m_render->DrawVertexArray( 3, m_startVerts );

	// Draw Text
	std::vector<Vertex> textStarshipDropShadowVerts;
	AddVertsForTextTriangles2D( textStarshipDropShadowVerts, "Starship", Vec2( 622.f, 699.f ), 40.f, Rgba8( 0, 153, 204 ) );
	g_engine->m_render->DrawVertexArray( (int)textStarshipDropShadowVerts.size(), textStarshipDropShadowVerts.data() );

	std::vector<Vertex> textStarshipVerts;
	AddVertsForTextTriangles2D( textStarshipVerts, "Starship", Vec2( 620.f, 700.f ), 40.f, Rgba8( 255, 25, 25 ) );
	g_engine->m_render->DrawVertexArray( (int)textStarshipVerts.size(), textStarshipVerts.data() );

	std::vector<Vertex> textGoldDropShadowVerts;
	AddVertsForTextTriangles2D( textGoldDropShadowVerts, "Gold", Vec2( 862.f, 699.f ), 40.f, Rgba8( 204, 25, 25 ) );
	g_engine->m_render->DrawVertexArray( (int)textGoldDropShadowVerts.size(), textGoldDropShadowVerts.data() );

	std::vector<Vertex> textGoldVerts;
	AddVertsForTextTriangles2D( textGoldVerts, "Gold", Vec2( 860.f, 700.f ), 40.f, Rgba8( 51, 204, 255 ) );
	g_engine->m_render->DrawVertexArray( (int)textGoldVerts.size(), textGoldVerts.data() );

	g_engine->m_render->EndCamera( *m_screenCamera );
}

void Game::UpdateStates()
{
	if( m_currentState != m_nextState )
	{
		if( m_currentState != GAME_STATE_INVALID )
		{
			g_engine->m_audio->StartSound( audio_selectSound );
		}
		if( m_nextState == GAME_STATE_PLAY )
		{
			m_music = g_engine->m_audio->StartSound( audio_music, true, 0.1f );
		}
		if( m_nextState == GAME_STATE_ATTRACT )
		{
			g_engine->m_audio->StopSound( m_music );
		}

		m_currentState = m_nextState;
	}
}

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
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

	ShakeCamera( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
void Game::CheckBulletVsEnemies()
{
	// Bullets Vs Asteroids
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

	// Bullets Vs Beetles
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

	// Bullets Vs Wasps
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

//-----------------------------------------------------------------------------------------------
void Game::CheckBulletVsAsteroid(Bullet& bullet, Asteroid& asteroid)
{
	if( DoEntitiesOverlap( bullet, asteroid ) )
	{
		bullet.TakeDamage( 1 );
		asteroid.TakeDamage( 1 );

		g_engine->m_audio->StartSound( audio_hurt, false, 0.025f );
		AddCameraShake( 0.5f );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckBulletVsBeetle( Bullet& bullet, Beetle& beetle )
{
	if( DoEntitiesOverlap( bullet, beetle ) )
	{
		bullet.TakeDamage( 1 );		
		if( bullet.m_isFireBullet && !beetle.m_isFireBeetle )
		{
			beetle.TakeDamage( 1 );
			g_engine->m_audio->StartSound( audio_hurt, false, 0.05f );
			AddCameraShake( 0.5f );

			if( beetle.m_isOnFire )
			{
				beetle.ResetFireTick();
			}
			else
			{
				beetle.m_isOnFire = true;
			}
		}
		else if( bullet.m_isIceBullet && !beetle.m_isIceBeetle )
		{
			beetle.TakeDamage( 1 );
			g_engine->m_audio->StartSound( audio_hurt, false, 0.05f );
			AddCameraShake( 0.5f );

			if( beetle.m_isSlow )
			{
				beetle.ResetSlowTimer();
			}
			else
			{
				beetle.m_isSlow = true;
			}
		}
		else if( !beetle.m_isFireBeetle && !beetle.m_isIceBeetle )
		{
			beetle.TakeDamage( 1 );
			g_engine->m_audio->StartSound( audio_hurt, false, 0.05f );
			AddCameraShake( 0.5f );
		}
		else
		{
			g_engine->m_audio->StartSound( audio_shield, false, 0.08f );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckBulletVsWasp( Bullet& bullet, Wasp& wasp )
{
	if( DoEntitiesOverlap( bullet, wasp ) )
	{
		bullet.TakeDamage( 1 );
		if( bullet.m_isFireBullet && !wasp.m_isFireWasp )
		{
			wasp.TakeDamage( 1 );
			g_engine->m_audio->StartSound( audio_hurt, false, 0.05f );
			AddCameraShake( 0.5f );

			if( wasp.m_isOnFire )
			{
				wasp.ResetFireTick();
			}
			else
			{
				wasp.m_isOnFire = true;
			}
		}
		else if( bullet.m_isIceBullet && !wasp.m_isIceWasp )
		{
			wasp.TakeDamage( 1 );
			g_engine->m_audio->StartSound( audio_hurt, false, 0.05f );
			AddCameraShake( 0.5f );

			if( wasp.m_isSlow )
			{
				wasp.ResetSlowTimer();
			}
			else
			{
				wasp.m_isSlow = true;
			}
		}
		else if( !wasp.m_isFireWasp && !wasp.m_isIceWasp )
		{
			wasp.TakeDamage( 1 );
			g_engine->m_audio->StartSound( audio_hurt, false, 0.05f );
			AddCameraShake( 0.5f );
		}
		else
		{
			g_engine->m_audio->StartSound( audio_shield, false, 0.08f );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckShipsVsEnemies()
{
	if( m_playerShip->m_isInvincible )
	{
		return;
	}

	// Ship Vs Asteroids
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid* asteroid = m_asteroids[ asteroidIndex ];
		if( asteroid && m_playerShip->IsAlive() )
		{
			CheckShipVsAsteroid( *asteroid, *m_playerShip );
		}
	}

	// Ship Vs Beetles
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle = m_beetles[ beetleIndex ];
		if( beetle && m_playerShip->IsAlive() )
		{
			CheckShipVsBeetle( *beetle, *m_playerShip );
		}
	}

	// Ship Vs Wasps
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp* wasp = m_wasps[ waspIndex ];
		if( wasp && m_playerShip->IsAlive() )
		{
			CheckShipVsWasp( *wasp, *m_playerShip );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckShipVsAsteroid(Asteroid& asteroid, PlayerShip& ship)
{
	if( DoEntitiesOverlap( asteroid, ship ) )
	{
		asteroid.TakeDamage( 3 );
		ship.TakeDamage( 1 ); 
		g_engine->m_audio->StartSound( audio_shipExplosion, false, 0.5f );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckShipVsBeetle( Beetle& beetle, PlayerShip& ship )
{
	if( DoEntitiesOverlap( beetle, ship ) )
	{
		ship.TakeDamage( 1 );
		g_engine->m_audio->StartSound( audio_shipExplosion, false, 0.5f );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckShipVsWasp( Wasp& wasp, PlayerShip& ship )
{
	if( DoEntitiesOverlap( wasp, ship ) )
	{
		ship.TakeDamage( 1 );
		g_engine->m_audio->StartSound( audio_shipExplosion, false, 0.5f );
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckEnemiesVsEnemies()
{
	// Beetles Vs Beetles
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

	// Wasps Vs Wasps
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp* wasp1 = m_wasps[ waspIndex ];
		for( int secondWaspIndex = waspIndex + 1; secondWaspIndex < MAX_WASPS; secondWaspIndex++ )
		{
			Wasp* wasp2 = m_wasps[ secondWaspIndex ];
			if( wasp1 && wasp2 )
			{
				CheckEnemyVsEnemy( *wasp1, *wasp2 );
			}
		}
	}

	// Beetles Vs Wasps
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle = m_beetles[ beetleIndex ];
		for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
		{
			Wasp* wasp = m_wasps[ waspIndex ];
			if( beetle && wasp )
			{
				CheckEnemyVsEnemy( *beetle, *wasp );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckEnemyVsEnemy( Entity& enemy1, Entity& enemy2 )
{
	if( DoEntitiesOverlap( enemy1, enemy2 ) )
	{
		PushDiscsOutOfEachOther2D( enemy1.m_position, enemy1.m_cosmeticRadius, enemy2.m_position, enemy2.m_cosmeticRadius );
	}
}


//-----------------------------------------------------------------------------------------------
void Game::CheckExplosionsVsEnemies()
{
	// Explosion Vs Beetles
	for( int explosionIndex = 0; explosionIndex < MAX_EXPLOSIONS; explosionIndex++ )
	{
		Explosion* explosion = m_explosions[ explosionIndex ];
		for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
		{
			Beetle* beetle = m_beetles[ beetleIndex ];
			if( explosion && beetle )
			{
				CheckExplosionsVsBeetle( *explosion, *beetle );
			}
		}
	}

	// Explosion Vs Wasps
	for( int explosionIndex = 0; explosionIndex < MAX_EXPLOSIONS; explosionIndex++ )
	{
		Explosion* explosion = m_explosions[ explosionIndex ];
		for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
		{
			Wasp* wasp = m_wasps[ waspIndex ];
			if( explosion && wasp )
			{
				CheckExplosionsVsWasp( *explosion, *wasp );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckExplosionsVsBeetle( Explosion& explosion, Beetle& beetle )
{
	if( DoEntitiesOverlap( explosion, beetle ) )
	{
		if( explosion.m_isFieryExplosion && !beetle.m_isFireBeetle )
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
		else if( explosion.m_isIcyExplosion && !beetle.m_isIceBeetle )
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

//-----------------------------------------------------------------------------------------------
void Game::CheckExplosionsVsWasp( Explosion& explosion, Wasp& wasp )
{
	if( DoEntitiesOverlap( explosion, wasp ) )
	{
		if( explosion.m_isFieryExplosion && !wasp.m_isFireWasp )
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
		else if( explosion.m_isIcyExplosion && !wasp.m_isIceWasp )
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

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
void Game::ShakeCamera( float deltaSeconds )
{
	float shakeX = g_rng->RollRandomFloatInRange( -m_screenShakeAmount, m_screenShakeAmount );
	float shakeY = g_rng->RollRandomFloatInRange( -m_screenShakeAmount, m_screenShakeAmount );

	m_worldCamera->Translate2D( Vec2( shakeX, shakeY ) );

	m_screenShakeAmount -= SCREEN_SHAKE_REDUCTION * deltaSeconds;
	m_screenShakeAmount = GetClamped( m_screenShakeAmount, 0.f, MAX_SCREEN_SHAKE_AMOUNT );

	if( m_screenShakeAmount <= 0.f )
	{
		m_isShaking = false;
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckForGameOver()
{
	if( this == nullptr || m_playerShip == nullptr )
	{
		return;
	}

	if( m_playerShip->m_isDead && m_playerShip->m_lives <= 0 )
	{
		g_engine->m_audio->StartSound( audio_gameOver, false, 0.1f, 0.f, 2.f );

		Debris* debris = m_debris[0];
		if( !debris->IsAlive() )
		{
			if( m_endGameTimer < 60 )
			{
				m_endGameTimer++;
			}
			else
			{
				m_nextState = GAME_STATE_ATTRACT;
				g_app->RestartGame();
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::CheckIfWaveNeedsToSpawn()
{
	if( this == nullptr )
	{
		return;
	}

	bool isThereBeetle = false;
	bool isThereWasp = false;	

	// Check if there are Beetles alive
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle* beetle = m_beetles[ beetleIndex ];
		if( beetle->IsAlive() )
		{
			isThereBeetle = true;
			break;
		}
	}

	// Check if there are Wasps alive
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
			g_engine->m_audio->StartSound( audio_waveStart, false, 0.5f );
			SpawnWave();
		}
		else
		{
			g_engine->m_audio->StartSound( audio_victory, false, 0.1f, 0.f, 3.f );
			if( m_endGameTimer < 320 )
			{
				m_endGameTimer++;
			}
			else
			{
				m_nextState = GAME_STATE_ATTRACT;
				g_app->RestartGame();
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::SpawnWave()
{
	// Spawn Asteroids
	for( int i = 0; i < m_numFieryAsteroidsPerWave[ m_waveNumber ]; i++ )
	{
		SpawnNewRandomFieryAsteroid();
	}

	for( int i = 0; i < m_numIcyAsteroidsPerWave[ m_waveNumber ]; i++ )
	{
		SpawnNewRandomIcyAsteroid();
	}

	// Spawn Beetles
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

	// Spawn Wasps
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

//-----------------------------------------------------------------------------------------------
bool Game::DoEntitiesOverlap(Entity const& a, Entity const& b)
{
	float dx = b.m_position.x - a.m_position.x;
	float dy = b.m_position.y - a.m_position.y;
	float distanceSquared = ( dx * dx ) + ( dy * dy );
	float combinedRadii = a.m_physicsRadius + b.m_physicsRadius;
	float radiiSquared = combinedRadii * combinedRadii;
	return distanceSquared < radiiSquared;
}

//-----------------------------------------------------------------------------------------------
void Game::DrawPlayerLives() const
{
	g_engine->m_render->EndCamera( *m_worldCamera );
	g_engine->m_render->BeginCamera( *m_screenCamera );

	Vertex verts[ NUM_SHIP_VERTS ];
	CreateFakePlayerShip( verts, 127 );

	if( m_playerShip->m_lives > 1 )
	{
		TransformVertexArrayXY3D( NUM_SHIP_VERTS, verts, 8.f, 90.f, Vec2( 20.f, 780.f ) );
		g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, verts );
	}

	for( int currentLife = 1; currentLife < m_playerShip->m_lives - 1; currentLife++ )
	{
		TransformVertexArrayXY3D( NUM_SHIP_VERTS, verts, 1.f, 0.f, Vec2( 40.f, 0.f ) );
		g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, verts );
	}

	g_engine->m_render->EndCamera( *m_screenCamera );
	g_engine->m_render->BeginCamera( *m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
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

		vert1Near.m_pos = Vec3( starX + 0.5f, starY, -0.f );
		vert2Near.m_pos = Vec3( starX - 0.5f, starY, 0.f );
		vert3Near.m_pos = Vec3( starX, starY + 0.5f, 0.f );
		vert4Near.m_pos = Vec3( starX + 0.5f, starY, 0.f );
		vert5Near.m_pos = Vec3( starX - 0.5f, starY, 0.f );
		vert6Near.m_pos = Vec3( starX, starY - 0.5f, 0.f );

		vert1Far.m_pos = Vec3( starX + 0.5f, starY, -0.f );
		vert2Far.m_pos = Vec3( starX - 0.5f, starY, 0.f );
		vert3Far.m_pos = Vec3( starX, starY + 0.5f, 0.f );
		vert4Far.m_pos = Vec3( starX + 0.5f, starY, 0.f );
		vert5Far.m_pos = Vec3( starX - 0.5f, starY, 0.f );
		vert6Far.m_pos = Vec3( starX, starY - 0.5f, 0.f );
	}
}

//-----------------------------------------------------------------------------------------------
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

void Game::UpdateKeyboardInput()
{
	if( m_currentState == GAME_STATE_ATTRACT && g_engine->m_input->WasKeyJustPressed( ' ' ) || g_engine->m_input->WasKeyJustPressed( 'N' ) )
	{
		m_nextState = GAME_STATE_PLAY;
	}

	if( m_currentState == GAME_STATE_ATTRACT && g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) )
	{
		g_app->m_isQuitting = true;
	}

	if( m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) )
	{
		m_nextState = GAME_STATE_ATTRACT;
	}

	if( m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( KEYCODE_F1 ) )
	{
		if( !m_debugDraw )
		{
			m_debugDraw = true;
		}
		else
		{
			m_debugDraw = false;
		}
	}

	if( m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		g_app->RestartGame();
	}

	if( m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( 'B' ) )
	{
		DebugGarbageAllEnemies();
	}
}

void Game::UpdateControllerInput()
{
	XboxController const& controller = g_engine->m_input->m_controllers[ 0 ];

	if( m_currentState == GAME_STATE_ATTRACT && ( controller.WasButtonJustPressed( XboxButtonID::START ) || controller.WasButtonJustPressed( XboxButtonID::A ) ) )
	{
		m_nextState = GAME_STATE_PLAY;
	}

	if( m_currentState == GAME_STATE_ATTRACT && controller.WasButtonJustPressed( XboxButtonID::SELECT ) )
	{
		g_app->m_isQuitting = true;
	}

	if( m_currentState == GAME_STATE_PLAY && controller.WasButtonJustPressed( XboxButtonID::SELECT ) )
	{
		m_nextState = GAME_STATE_ATTRACT;
	}
}

void Game::InitializeStartTriangleVerts()
{
	m_startVerts[ 0 ].m_pos = Vec3( -200.f, 200.f, 0.f );
	m_startVerts[ 1 ].m_pos = Vec3( -200.f, -200.f, 0.f );
	m_startVerts[ 2 ].m_pos = Vec3( 200.f, 0.f, 0.f );

	for( int vertIndex = 0; vertIndex < 3; vertIndex++ )
	{
		m_startVerts[ vertIndex ].m_color = Rgba8( 0, 255, 0 );
	}
}

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
void Game::DebugDrawWorldBounds() const
{
	DebugDrawLine( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, WORLD_SIZE_Y ), Vec2( WORLD_SIZE_X, 0.f ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, 0.f ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, WORLD_SIZE_Y ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( 0.f, 0.f ), Vec2( 0.f, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
	DebugDrawLine( Vec2( WORLD_SIZE_X, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ), 5.f, Rgba8( 255, 0, 255 ) );
}

//-----------------------------------------------------------------------------------------------
void Game::DebugGarbageAllEnemies()
{
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle*& beetle = m_beetles[ beetleIndex ];

		if( beetle )
		{
			beetle->m_isGarbage = true;
		}
	}

	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp*& wasp = m_wasps[ waspIndex ];
		if( wasp )
		{
			wasp->m_isGarbage = true;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game::DeleteGarbageEntities()
{
	// Delete Asteroids
	for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++ )
	{
		Asteroid*& asteroid = m_asteroids[ asteroidIndex ];
		if( asteroid && asteroid->m_isGarbage )
		{
			delete asteroid;
			asteroid = nullptr;
		}
	}

	// Delete Bullets
	for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++ )
	{
		Bullet*& bullet = m_bullets[ bulletIndex ];
		if( bullet && bullet->m_isGarbage )
		{
			delete bullet;
			bullet = nullptr;
		}
	}

	// Delete Beetles
	for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++ )
	{
		Beetle*& beetle = m_beetles[ beetleIndex ];
		if( beetle && beetle->m_isGarbage )
		{
			delete beetle;
			beetle = nullptr;
		}
	}

	// Delete Wasps
	for( int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++ )
	{
		Wasp*& wasp = m_wasps[ waspIndex ];
		if( wasp && wasp->m_isGarbage )
		{
			delete wasp;
			wasp = nullptr;
		}
	}

	// Delete Debris
	for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++ )
	{
		Debris*& debris = m_debris[ debrisIndex ];
		if( debris && debris->m_isGarbage )
		{
			delete debris;
			debris = nullptr;
		}
	}

	// Delete Explosions
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