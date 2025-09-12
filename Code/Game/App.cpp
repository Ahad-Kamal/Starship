#include "App.hpp"
#include "PlayerShip.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"

App* g_app = nullptr;

Rgba8* clearColor = new Rgba8();

Vertex* vertexes = new Vertex();

App::App()
{
	g_engine = new Engine();

	m_ship1 = new PlayerShip(Vec2(0.f, 30.f), Vec2(12.f, 0.f));
	m_ship2 = new PlayerShip(Vec2(0.f, 50.f), Vec2(20.f, 0.f));
	m_ship3 = new PlayerShip(Vec2(0.f, 70.f), Vec2(15.f, 0.f));
}

App::~App()
{
	delete g_engine;
	g_engine = nullptr;

	delete m_ship1;
	m_ship1 = nullptr;

	delete m_ship2;
	m_ship2 = nullptr;

	delete m_ship3;
	m_ship3 = nullptr;
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

	m_ship1->Update(deltaSeconds);
	m_ship2->Update(deltaSeconds);
	m_ship3->Update(deltaSeconds);

	if (m_ship1->m_position.x > 200.f ||
		m_ship2->m_position.x > 200.f ||
		m_ship3->m_position.x > 200.f)
	{
		m_isQuitting = true;
	}
}

void App::Render() const
{
	g_engine->m_render->BeginCamera(*g_engine->m_camera);
	
	g_engine->m_render->ClearScreen(*clearColor); // note to self, clearColor is null, fine for now since its not currently in use but remember this for later

	//g_engine->m_render->DrawVertexArray(3, vertexes); // note to self, vertexes is null, fine for now since its not currently in use but remember this for later
	m_ship1->Render();
	m_ship2->Render();
	m_ship3->Render();

	g_engine->m_render->EndCamera(*g_engine->m_camera);
}

void App::SetIsQuitting()
{

}

bool App::IsQuitting() const
{
	return false;
}

void App::OnKeyDown(unsigned char keyCode)
{
	// isKeyDownArray[keyCode] = true
}

void App::OnKeyUp(unsigned char keyCode)
{
	// isKeyDownArray[keyCode] = false && isKeyDownPrevArray = true
}

