#pragma once
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Camera.hpp"

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
	bool m_isAttractMode = true;

private:
	Vertex m_startVerts[ 3 ];
};