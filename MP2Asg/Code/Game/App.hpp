#pragma once
#include "Engine/Math/vec2.hpp"
#include "Game/GameCommon.hpp"
#include <windows.h>


class Game;


class App {
public:
	App(){}
	~App() {}
	void Startup();
	void StartupStage1();	// Critial system
	void StartupStage2();	// Other system dependencies 
	void StartupStage3();	// Object
	void Shutdown();
	void BeginFrame();
	void RunFrame();
	void EndFrame();
	bool IsQuitting() const { return m_isQuitting; }
	void CheckGameQuit();
	void HandleQuitRequested();
	void HandleKeyPressed( unsigned char inValue );
	void HandleKeyReleased( unsigned char inValue );
	void ResetGame();
	bool DoesUseIMGUI() const { return false; }
	void handleIMGUIInput( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam );

private:
	void Update( float deltaSeconds );
	const void Render() const;
	
private:
	bool m_isQuitting		= false;
	float m_deltaTime		= 0;
	float m_timeFraction	= 1.f;
};