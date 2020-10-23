#pragma once
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/vec2.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/ObjectReader.hpp"

class Game;
class Clock;

class App {
public:
	App(){}
	~App() {}
	void Startup();

	// test function
	void TemplateTesting();
	void StringTesting();
	void NamedPropertyTesting();
	void BlockingQueueTesting();
	void HandleNetworkMsg();

	void Shutdown();
	void BeginFrame();
	void RunFrame();
	void EndFrame();
	bool IsQuitting() const { return m_isQuitting;  }
	void CheckGameQuit();
	void HandleQuitRequested();
	void HandleDevConsoleInput();
	void ResetGame();
	std::string GetWindowsName() const { return m_windowName; }


private:
	void Update( float deltaSeconds );
	void UpdateDevConsole( float deltaSeconds );
	const void Render() const;
	
private:
	bool m_isQuitting		= false;
	float m_deltaTime		= 0;
	float m_timeFraction	= 1.f;
	std::string m_windowName;
	Clock* m_clock = nullptr;
};

bool HelpCommandEvent( EventArgs& args );
bool QuitCommandEvent( EventArgs& args );
bool ConnectToMultiplayerServer( EventArgs& args );
bool StartMultiplayerServer( EventArgs& args );