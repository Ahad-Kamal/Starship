#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/GameCommon.hpp"

#include <math.h>

App* g_app = nullptr;

Rgba8 g_clearColor = Rgba8( 0.f, 0.f, 0.f, 1.f );

App::App()
{
	g_engine = new Engine();
	m_game = new Game( this );

	m_lastFrameTime = static_cast<float>( GetCurrentTimeSeconds() );
	InitializeStartTriangleVerts();
	TransformVertexArrayXY3D( 3, m_startVerts, 1.f, 0.f, Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ) );
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

void App::Update(float deltaSeconds)
{
	CheckKeyboardInput();
	CheckControllerInput();
	g_engine->m_input->BeginFrame();

	if( m_isAttractMode )
	{
		m_time += deltaSeconds * 1.25f;
		m_startAlpha = 127.5f * cosf( m_time * 2.0f ) + 127.5f;

		for( int startIndex = 0; startIndex < 3; startIndex++ )
		{
			m_startVerts[ startIndex ].m_color = Rgba8( 0.f, 255.f, 0.f, m_startAlpha );
		}
		return;
	}

	if (m_isPaused)
	{
		deltaSeconds = 0;
	}

	if (m_isSlowMo)
	{
		deltaSeconds *= 0.1f;
	}

	m_game->Update( deltaSeconds );

	//UpdateKeyStates();
}

void App::Render() const
{
	if( m_isAttractMode )
	{
		RenderAttractMode();
		return;
	}

	//g_engine->m_render->BeginCamera(*g_engine->m_camera);
	g_engine->m_render->BeginCamera( *m_game->m_worldCamera );
	
	g_engine->m_render->ClearScreen( g_clearColor ); // note to self, clearColor is null, fine for now since its not currently in use but remember this for later

	m_game->Render();

	//g_engine->m_render->EndCamera(*g_engine->m_camera);
	g_engine->m_render->EndCamera( *m_game->m_worldCamera );
}

void App::RenderAttractMode() const
{
	Camera attractCamera;
	attractCamera.SetOrthoView( Vec2( 0.f, 0.f), Vec2( 200.f, 100.f ) );
	g_engine->m_render->BeginCamera( attractCamera );

	g_engine->m_render->ClearScreen(g_clearColor); // note to self, clearColor is null, fine for now since its not currently in use but remember this for later

	Vertex shipVerts[ NUM_SHIP_VERTS ];
	createFakePlayerShip( shipVerts, 255 );

	TransformVertexArrayXY3D( NUM_SHIP_VERTS, shipVerts, 8.f, 0.f, Vec2( 50.f, 50.f ) );
	g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, shipVerts );

	TransformVertexArrayXY3D( NUM_SHIP_VERTS, shipVerts, 1.f, 0.f, Vec2( -50.f, -50.f ) );
	TransformVertexArrayXY3D( NUM_SHIP_VERTS, shipVerts, 1.f, 180.f, Vec2( 150.f, 50.f ) );
	g_engine->m_render->DrawVertexArray( NUM_SHIP_VERTS, shipVerts );

	g_engine->m_render->DrawVertexArray( 3, m_startVerts );

	g_engine->m_render->EndCamera( attractCamera );
}

void App::SetIsQuitting()
{

}

bool App::IsQuitting() const
{
	return false;
}

void App::CheckKeyboardInput()
{
	if( m_isAttractMode && g_engine->m_input->wasKeyJustPressed( ' ' ) || g_engine->m_input->wasKeyJustPressed( 'N' ) )
	{
		m_isAttractMode = false;
	}

	if( !m_isAttractMode && g_engine->m_input->wasKeyJustPressed( KEYCODE_ESC ) )
	{
		m_isAttractMode = true;
	}
	
	if( !m_isAttractMode && g_engine->m_input->wasKeyJustPressed( KEYCODE_F1 ) )
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

	if( !m_isAttractMode && g_engine->m_input->wasKeyJustPressed( KEYCODE_F8 ) )
	{
		RestartGame();
	}

}

void App::CheckControllerInput()
{
	XboxController const& controller = g_engine->m_input->m_controllers[ 0 ];

	if( m_isAttractMode && ( controller.WasButtonJustPressed( XboxButtonID::START ) || controller.WasButtonJustPressed( XboxButtonID::A ) ) )
	{
		m_isAttractMode = false;
	}

	if( !m_isAttractMode && controller.WasButtonJustPressed( XboxButtonID::SELECT ) )
	{
		m_isAttractMode = true;
	}
}

void App::InitializeStartTriangleVerts() 
{
	m_startVerts[ 0 ].m_pos = Vec3( -20.f, 20.f, 0.f);
	m_startVerts[ 1 ].m_pos = Vec3( -20.f, -20.f, 0.f );
	m_startVerts[ 2 ].m_pos = Vec3( 20.f, 0.f, 0.f );

	for( int vertIndex = 0; vertIndex < 3; vertIndex++ )
	{
		m_startVerts[ vertIndex ].m_color = Rgba8( 0, 255, 0 );
	}
}

void App::RestartGame()
{
	m_game->Shutdown();

	delete m_game;
	m_game = nullptr;

	m_game = new Game( this );
}

