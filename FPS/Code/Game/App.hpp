#pragma once
#include "Engine/Core/Event/EventSystem.hpp"
#include "Engine/Math/vec2.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Obj/ObjectReader.hpp"

class Game;


class App {
public:
	App(){}
	~App() {}
	void Startup();

	// test function
	void TemplateTesting();
	void StringTesting();
	void NamedPropertyTesting();

	void Shutdown();
	void BeginFrame();
	void RunFrame();
	void EndFrame();
	bool IsQuitting() const { return m_isQuitting;  }
	void CheckGameQuit();
	void HandleQuitRequested();
	void HandleDevConsoleInput();
	void ResetGame();

private:
	void Update( float deltaSeconds );
	void UpdateDevConsole( float deltaSeconds );
	const void Render() const;
	
private:
	bool m_isQuitting		= false;
	float m_deltaTime		= 0;
	float m_timeFraction	= 1.f;
};

bool HelpCommandEvent( EventArgs& args );
bool QuitCommandEvent( EventArgs& args );
bool eventTest( EventArgs& args );