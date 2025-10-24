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

	void SetIsQuitting();
	bool IsQuitting() const;
	void UpdateKeyboardInput();
	void RestartGame();

public:
	Game* m_game = nullptr;
	float m_lastFrameTime = 0.f;
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_pauseNextFrame = false;
	bool m_isSlowMo = false;
	bool m_pauseAfterNextUpdate = false;
};