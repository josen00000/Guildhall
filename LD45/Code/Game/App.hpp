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
	void RunFrame();

	bool IsQuitting() const { return m_isQuitting; }
	//bool HandleKeyPressed( unsigned char keyCode );
	//bool HandleKeyReleased( unsigned char keyCode );
	void HandleQuitRequested();
	void CheckGameQuit();
	void HandleKeyPressed(unsigned char inValue);
	void HandleKeyReleased(unsigned char inValue);
	

/************************************************************************/
/*                                                                      */
/************************************************************************/
private:

	void BeginFrame();
	void Update( float deltaSeconds );
	const void Render() const;
	void EndFrame();
	

private:
	float m_deltaTime=0;
	bool m_isQuitting=false;
	bool m_isPaused=false;
	bool m_isSlowMo=false;
	bool m_isDevelopMode=false;
	Vec2 m_shipPos;
	Game* m_theGame=nullptr;
	float m_timeFraction=1.f;

};