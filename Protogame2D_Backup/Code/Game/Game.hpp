#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


class RenderContext;
class InputSystem;
class Camera;
class Entity;
class World;


class Game {
public:
	Game(){}
	~Game(){}
	Game(  bool isDevelop,Camera* inCamera);

	//basic
	void Startup();
	void Shutdown();
	void RunFrame(float deltaTime);
	void CheckGameStates();
	void CheckIfPause();
	void CheckIfDeveloped();
	void CheckIfDebugNoClip();
	void Render() const;
	void ControlCollision();
	void RenderUI() const;
	void Reset();
	void UpdateCamera(float deltaTime);
	void UpdateWorld(float deltaTime);
	
private:
	void Update(float deltaTIme);
	void TestSprite();

public:
	bool m_developMode = false;
	bool m_noClip = false;
	bool m_isPause = false;
	bool m_debugCamera = false;
	bool isAppQuit = false;
	bool m_isAttractMode = false;
	//should using global.
	
	Camera* m_camera = nullptr;
	RandomNumberGenerator m_rng;
	World* m_world = nullptr;
	int m_numTilesInViewVertically=16*9;
};
extern Game* g_theGame;