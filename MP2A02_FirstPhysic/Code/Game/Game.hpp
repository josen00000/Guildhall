#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"


class RenderContext;
class InputSystem;
class RandomNumberGenerator;
class Camera;
class Entity;
class World;

struct Vertex_PCU;

enum GameState {
	GAME_STATE_NULL = -1,
	GAME_STATE_LOADING,
	GAME_STATE_ATTRACT,
	GAME_STATE_PLAYING,
	GAME_STATE_END,
	NUM_GAME_STATE
};

class Game {
public:
	Game(){}
	~Game(){}
	explicit Game( Camera* gameCamera, Camera* UICamera );

	//basic
	void Startup();
	void Shutdown();
	void RunFrame( float deltaSeclnds );
	void EndFrame();
	void Reset();
	void Render() const;
	void RenderUI() const;

private:
	void Update( float deltaSeconds );
	void UpdateUI( float deltaSeconds );
	void UpdateCamera(float deltaSeconds );

	//Game State Check
	void CheckIfExit();
	void CheckGameStates();
	void CheckIfPause();
	void CheckIfDeveloped();
	void CheckIfNoClip();

	//Load
	void LoadAssets();


public:
	bool m_developMode		= false;
	bool m_noClip			= false;
	bool m_isPause			= false;
	bool m_debugCamera		= false;
	bool m_isAppQuit		= false;
	bool m_isAttractMode	= false;

	GameState m_gameState	= GAME_STATE_NULL;
	World*	m_world			= nullptr;
	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;
	RandomNumberGenerator* m_rng = nullptr;
	
	std::vector<Vertex_PCU> m_UIVertices;

};
