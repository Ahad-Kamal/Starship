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

App* g_app = nullptr;
Rgba8 g_clearColor = Rgba8( 0, 0, 0, 1 );

App::App()
{
	g_engine = new Engine();
	m_game = new Game( this );

	CreateSounds();
	m_lastFrameTime = static_cast<float>( GetCurrentTimeSeconds() );
	InitializeStartTriangleVerts();
	createFakePlayerShip( m_fakeShipVerts, 255 );
	TransformVertexArrayXY3D( 3, m_startVerts, 1.f, 0.f, Vec2( SCREEN_CENTER_X, SCREEN_CENTER_Y ) );
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
	if( m_currentState != m_nextState )
	{	
		if( m_currentState != GAME_STATE_INVALID )
		{
			g_engine->m_audio->StartSound( audio_selectSound );
		}
		m_currentState = m_nextState;
	}

	CheckKeyboardInput();
	CheckControllerInput();
	g_engine->m_input->BeginFrame();

	if( m_currentState == GAME_STATE_ATTRACT )
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
			m_startVerts[ startIndex ].m_color = Rgba8( 0, 255, 0, (unsigned int) m_startAlpha );
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
}

void App::Render() const
{
	if( m_currentState == GAME_STATE_ATTRACT )
	{
		RenderAttractMode();
		return;
	}

	g_engine->m_render->BeginCamera( *m_game->m_worldCamera );
	
	g_engine->m_render->ClearScreen( g_clearColor );

	m_game->Render();

	g_engine->m_render->EndCamera( *m_game->m_worldCamera );
}

void App::RenderAttractMode() const
{
	g_engine->m_render->BeginCamera( *m_game->m_screenCamera );

	g_engine->m_render->ClearScreen( g_clearColor ); 

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
	AddVertsForTextTriangles2D( textStarshipVerts, "Starship", Vec2( 620.f, 700.f), 40.f, Rgba8( 255, 25, 25 ) );
	g_engine->m_render->DrawVertexArray( (int) textStarshipVerts.size(), textStarshipVerts.data() );

	std::vector<Vertex> textGoldDropShadowVerts;
	AddVertsForTextTriangles2D( textGoldDropShadowVerts, "Gold", Vec2( 862.f, 699.f ), 40.f, Rgba8( 204, 25, 25 ) );
	g_engine->m_render->DrawVertexArray( (int)textGoldDropShadowVerts.size(), textGoldDropShadowVerts.data() );

	std::vector<Vertex> textGoldVerts;
	AddVertsForTextTriangles2D( textGoldVerts, "Gold", Vec2( 860.f, 700.f ), 40.f, Rgba8( 51, 204, 255 ) );
	g_engine->m_render->DrawVertexArray( (int)textGoldVerts.size(), textGoldVerts.data() );

	g_engine->m_render->EndCamera( *m_game->m_screenCamera );
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
	if( m_currentState == GAME_STATE_ATTRACT && g_engine->m_input->wasKeyJustPressed( ' ' ) || g_engine->m_input->wasKeyJustPressed( 'N' ) )
	{
		m_nextState = GAME_STATE_PLAY;
	}

	if( m_currentState == GAME_STATE_PLAY && g_engine->m_input->wasKeyJustPressed( KEYCODE_ESC ) )
	{
		m_nextState = GAME_STATE_ATTRACT;
	}
	
	if( m_currentState == GAME_STATE_PLAY && g_engine->m_input->wasKeyJustPressed( KEYCODE_F1 ) )
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

	if( m_currentState == GAME_STATE_PLAY && g_engine->m_input->wasKeyJustPressed( KEYCODE_F8 ) )
	{
		RestartGame();
	}

}

void App::CheckControllerInput()
{
	XboxController const& controller = g_engine->m_input->m_controllers[ 0 ];

	if( m_currentState == GAME_STATE_ATTRACT && ( controller.WasButtonJustPressed( XboxButtonID::START ) || controller.WasButtonJustPressed( XboxButtonID::A ) ) )
	{
		m_nextState = GAME_STATE_PLAY;
	}

	if( m_currentState == GAME_STATE_ATTRACT && controller.WasButtonJustPressed( XboxButtonID::SELECT ) )
	{
		m_isQuitting = true;
	}

	if( m_currentState == GAME_STATE_PLAY && controller.WasButtonJustPressed( XboxButtonID::SELECT ) )
	{
		m_nextState = GAME_STATE_ATTRACT;
	}
}

void App::InitializeStartTriangleVerts() 
{
	m_startVerts[ 0 ].m_pos = Vec3( -200.f, 200.f, 0.f);
	m_startVerts[ 1 ].m_pos = Vec3( -200.f, -200.f, 0.f );
	m_startVerts[ 2 ].m_pos = Vec3( 200.f, 0.f, 0.f );

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

