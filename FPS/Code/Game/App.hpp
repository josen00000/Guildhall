#pragma once
#include<Engine/Math/vec2.hpp>
#include<Game/GameCommon.hpp>


class Game;


class App {
public:
	App(){}
	~App() {}
	void Startup();
	void Shutdown();
	void BeginFrame();
	void RunFrame();
	void EndFrame();
	bool IsQuitting() const { return true; /* m_isQuitting; */ }
	void CheckGameQuit();
	void HandleQuitRequested();
	void HandleKeyPressed( unsigned char inValue );
	void HandleKeyReleased( unsigned char inValue );
	void ResetGame();

private:
	void Update( float deltaSeconds );
	const void Render() const;
	
private:
	bool m_isQuitting		= false;
	float m_deltaTime		= 0;
	float m_timeFraction	= 1.f;
};