#include "App.hpp"
#include "PlayerShip.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "GameCommon.hpp"
#include "Game.hpp"

App* g_app = nullptr;

Rgba8* clearColor = new Rgba8();

//Vertex* vertexes = new Vertex();

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

	CheckFunctionInput();
	UpdateKeyStates();

	//m_ship1->Update(deltaSeconds);
	//m_ship2->Update(deltaSeconds);
	//m_ship3->Update(deltaSeconds);

	//if (m_ship1->m_position.x > 200.f //||
	//	m_ship2->m_position.x > 200.f ||
	//	m_ship3->m_position.x > 200.f
	//	)
	//{
	//	m_isQuitting = true;
	//}
}

void App::Render() const
{
	g_engine->m_render->BeginCamera(*g_engine->m_camera);
	
	g_engine->m_render->ClearScreen(*clearColor); // note to self, clearColor is null, fine for now since its not currently in use but remember this for later

	//g_engine->m_render->DrawVertexArray(3, vertexes); // note to self, vertexes is null, fine for now since its not currently in use but remember this for later
	//m_ship1->Render();
	//m_ship2->Render();
	//m_ship3->Render();

	//DebugDrawRing( Vec2( 50.f, 20.f ), 10.f, 2.f, Rgba8( 255, 100, 0) );

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
	
	if( wasKeyJustPressed( 'p' ) )
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

