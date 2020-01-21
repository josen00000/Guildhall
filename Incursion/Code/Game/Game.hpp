#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/AABB2.hpp"

class RenderContext;
class InputSystem;
class Camera;
class Entity;
class World;
class DevConsole;

enum GameState
{
	GAME_STATE_LOADING,
	GAME_STATE_ATTRACT,
	GAME_STATE_PLAYING,
	GAME_STATE_VICTORY,
	GAME_STATE_PAUSED,
	NUM_GAME_STATE
	
};

class Game {
public:
	Game(){}
	~Game(){}
	Game(  bool isDevelop,Camera* inCamera);

	//basic
	void Startup();
	void Shutdown();
	void RunFrame(float deltaSeconds);
	void Reset();
	void CreatePauseScene();
	
	//Update
	void Update(float deltaSeconds);
	void UpdateWorld(float deltaSeconds);
	//void UpdateXboxVibration(float deltaSeconds); //does not work
	void UpdateLoadingScene( float deltaSeconds );
	void UpdateAttractScene( float deltaSeconds );
	
	//Render
	void Render() const;
	void RenderUI() const;
	void RenderLoadingScene() const;
	void RenderAttractScene() const;
	void RenderPauseScene() const;

	//Load
	void LoadGameAsset();

	//Check
	void CheckGameStates();
	void CheckIfPause();
	void CheckIfExit();
	void CheckIfDeveloped();
	void CheckIfDebugNoClip();
	//void RenderTestDevConsole() const;
	
private:
	//void TestDevConsole();

public:
	bool m_developMode = false;
	bool m_noClip = false;
	bool m_isPause = false;
	bool m_isPlayerDead = false;
	bool m_resetFinish = false;
	bool m_debugCamera = false;
	bool isAppQuit = false;
	bool m_isLoadingSceneRender = false;
	float m_vibrationIntense = 0;
	float m_vibrationDownRate = 0.5;
	AABB2 m_pauseScene;
	Rgba8 m_pauseColor;
	std::vector<Vertex_PCU> m_pauseText;
	GameState m_gameState = GAME_STATE_LOADING;
	//should using global.
	std::vector<Vertex_PCU> m_LoadingTextVertices;
	std::vector<Vertex_PCU> m_attractTextVertices;
	Camera* m_camera = nullptr;
	RandomNumberGenerator m_rng;
	World* m_world = nullptr;
	DevConsole* m_testConsole = nullptr;
};
