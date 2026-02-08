#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/GameCommon.hpp"
#include <math.h>


//-----------------------------------------------------------------------------------------------
App* g_app = nullptr;
Rgba8 g_clearColor = Rgba8( 0, 0, 0, 1 );


//-----------------------------------------------------------------------------------------------
App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.f;
	config.m_windowConfig.m_windowTitle = "Starship Gold";

	g_engine = new Engine( config );
	m_game = new Game( this );
	m_lastFrameTime = static_cast<float>( GetCurrentTimeSeconds() );

	CreateSounds();
}

//-----------------------------------------------------------------------------------------------
App::~App()
{
	delete m_game;
	m_game = nullptr;

	delete g_engine;
	g_engine = nullptr;
}

//-----------------------------------------------------------------------------------------------
void App::RunFrame()
{
	float timeNow = static_cast<float>( GetCurrentTimeSeconds() );
	float deltaSeconds = timeNow - m_lastFrameTime;
	m_lastFrameTime = timeNow;
	deltaSeconds = GetClamped( deltaSeconds, 0.f, 0.1f );

	// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
	g_engine->BeginFrame(); // Allow engine subsystems to do pre-frame stuff
	Update( deltaSeconds );		
	Render();		
	g_engine->EndFrame(); // Allow engine subsystems to do post-frame stuff
}

//-----------------------------------------------------------------------------------------------
void App::Update(float deltaSeconds)
{
	g_engine->m_input->BeginFrame();


	if( m_pauseNextFrame )
	{
		m_isPaused = true;
	}
	UpdateKeyboardInput();

	if( m_isPaused )
	{
		deltaSeconds = 0;
	}

	if( m_isSlowMo )
	{
		deltaSeconds *= 0.1f;
	}

	m_game->Update( deltaSeconds );
}

void App::Render() const
{
	g_engine->m_render->BeginCamera( *m_game->m_worldCamera );
	
	g_engine->m_render->ClearScreen( g_clearColor );

	m_game->Render();

	g_engine->m_render->EndCamera( *m_game->m_worldCamera );
}

void App::SetIsQuitting()
{

}

bool App::IsQuitting() const
{
	return m_isQuitting;
}

void App::UpdateKeyboardInput()
{
	if( m_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		RestartGame();
	}

	if( m_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( 'P' ) )
	{
		if( !m_isPaused )
		{
			m_isPaused = true;
		}
		else
		{
			m_isPaused = false;
			m_pauseNextFrame = false;
		}
	}

	if( m_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( 'T' ) )
	{
		m_isSlowMo = true;
	}

	if( m_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustReleased( 'T' ) )
	{
		m_isSlowMo = false;
	}

	if( m_game->m_currentState == GAME_STATE_PLAY && g_engine->m_input->WasKeyJustPressed( 'O' ) )
	{
		if( !m_isPaused )
		{
			m_pauseNextFrame = true;
		}
		else
		{
			m_pauseNextFrame = true;
			m_isPaused = false;
		}
	}

}

void App::RestartGame()
{
	m_game->Shutdown();

	delete m_game;
	m_game = nullptr;

	m_game = new Game( this );
}

