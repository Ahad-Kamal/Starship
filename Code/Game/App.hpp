#pragma once
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Camera.hpp"


class PlayerShip;
class App;
class Game;

extern App* g_app;

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
	void RestartGame();
	void CheckFunctionInput();

public:
	Game* m_game = nullptr;
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_isSlowMo = false;
	bool m_pauseAfterNextUpdate = false;
	bool m_debugDraw = false;
	bool m_isAttractMode = true;
};