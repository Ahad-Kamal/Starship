#pragma once
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"


//-----------------------------------------------------------------------------------------------
class App;
class Game;
extern App* g_app;

//-----------------------------------------------------------------------------------------------
class App
{
public:
	App();
	~App();

	void RunFrame();
	void Update( float deltaSeconds );
	void Render() const;
	void RenderAttractMode() const;

	void SetIsQuitting();
	bool IsQuitting() const;
	void RestartGame();
	void CheckKeyboardInput();
	void CheckControllerInput();

private:
	void InitializeStartTriangleVerts();

public:
	Game* m_game = nullptr;
	float m_lastFrameTime = 0.f;
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_isSlowMo = false;
	bool m_pauseAfterNextUpdate = false;
	bool m_debugDraw = false;
	GameState m_currentState = GAME_STATE_INVALID;
	GameState m_nextState = GAME_STATE_ATTRACT;

private:
	Vertex m_startVerts[ 3 ];
	Vertex m_fakeShipVerts[ NUM_SHIP_VERTS ];
	float m_startAlpha = 255.f;
	float m_shipRotation = 0.f;
	float m_time = 0.f;
	SoundPlaybackID m_music;
};