#pragma once
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/vec2.hpp"
#include "Game/GameCommon.hpp"


class Game;


class App {
public:
	App(){}
	~App() {}
	void Startup();
	void TemplateTesting();
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