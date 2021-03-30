#pragma once
#include<Engine/Math/vec2.hpp>
#include<Game/GameCommon.hpp>


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
	void ResetGame();
	void PauseGame();
	void UnPauseGame();

	void SonyTest();

private:
	void Update( float deltaSeconds );
	const void Render() const;
	
private:
	bool m_isQuitting		= false;
	bool m_isPauseTime		= false;
	float m_timeFraction	= 1.f;
};