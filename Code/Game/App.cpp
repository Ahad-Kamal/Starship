#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/Renderer.hpp"

App* g_app = nullptr;

Rgba8 g_clearColor = Rgba8();

App::App()
{
	g_engine = new Engine();
	m_game = new Game( this );
}

App::~App()
{
	delete m_game;
	m_game = nullptr;

	delete g_engine;
	g_engine = nullptr;
}

void App::RunFrame()
{

}

void App::Update(float deltaSeconds)
{
	if (m_isPaused)
	{
		deltaSeconds = 0;
	}

	if (m_isSlowMo)
	{
		deltaSeconds *= 0.1f;
	}

	m_game->Update( deltaSeconds );

	//CheckFunctionInput();
	//UpdateKeyStates();
}

void App::Render() const
{
	g_engine->m_render->BeginCamera(*g_engine->m_camera);
	
	g_engine->m_render->ClearScreen(g_clearColor); // note to self, clearColor is null, fine for now since its not currently in use but remember this for later

	m_game->Render();

	g_engine->m_render->EndCamera(*g_engine->m_camera);
}

void App::SetIsQuitting()
{

}

bool App::IsQuitting() const
{
	return false;
}

void App::CheckFunctionInput()
{
	
	if( wasKeyJustPressed( FUNCTION_KEY_1 ) )
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

	if( wasKeyJustPressed( FUNCTION_KEY_8 ) )
	{
		RestartGame();
	}

}

void App::RestartGame()
{
	m_game->Shutdown();

	delete m_game;
	m_game = nullptr;

	m_game = new Game( this );
}

void App::UpdateKeyStates()
{
	for( int i = 0; i < 256; ++i )
	{
		wasKeyDownPrev[i] = isKeyDownNow[i];
	}
}

void App::OnKeyDown(unsigned char keyCode)
{
	if( !isKeyDownNow[keyCode] )
	{
		isKeyDownNow[keyCode] = true;
	}
}

void App::OnKeyUp(unsigned char keyCode)
{
	if( isKeyDownNow[keyCode] )
	{
		isKeyDownNow[keyCode] = false;
	}
}

bool App::isKeyDown( unsigned char keyCode ) const
{
	if( isKeyDownNow[keyCode] )
	{
		return true;
	}
	
	return false;

}

bool App::wasKeyJustPressed( unsigned char keyCode ) const
{
	if( isKeyDownNow[keyCode] == true && wasKeyDownPrev[keyCode] == false )
	{
		return true;
	}

	return false;
}

