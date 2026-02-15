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
	void Update();
	void Render() const;

	void SetIsQuitting( bool quit );
	bool IsQuitting() const;

	static bool Event_Quit( EventArgs& args );

	void UpdateKeyboardInput();
	void RestartGame();

public:
	float m_lastFrameTime = 0.f;
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_pauseNextFrame = false;
	bool m_isSlowMo = false;
	bool m_isFastMo = false;
	bool m_pauseAfterNextUpdate = false;
};