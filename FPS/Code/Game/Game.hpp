#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Engine/Math/vec2.hpp"


class Camera;
class InputSystem;
class GPUMesh;
class RenderContext;
class RandomNumberGenerator;

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
	void Reset();
	void Render() const;
	void RenderSpheres() const;
	void RenderUI() const;

	
private:
	void Update( float deltaSeconds );
	void UpdateUI( float deltaSeconds );
	void UpdateCamera(float deltaSeconds );
	void UpdateMeshes( float deltaSeconds );
	void UpdateSphereMeshes( float deltaSeconds );

	void HandleKeyboardInput();
	void HandleMouseInput();
	void HandleCameraMovement();
		 
	//Game State Check
	void CheckIfExit();
	void CheckGameStates();
	void CheckIfPause();
	void CheckIfDeveloped();
	void CheckIfNoClip();

	//Load
	void LoadAssets();

	// Test
	void CreateTestMesh();
	void CreateSphereMesh();

public:
	bool m_developMode		= false;
	bool m_noClip			= false;
	bool m_isPause			= false;
	bool m_debugCamera		= false;
	bool m_isAppQuit		= false;
	bool m_isAttractMode	= false;
	
	GameState m_gameState	= GAME_STATE_NULL;
	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;
	RandomNumberGenerator* m_rng = nullptr;
	
	std::vector<Vertex_PCU> m_vertices;
	GPUMesh* m_meshCube = nullptr;
	std::vector<GPUMesh*> m_meshSpheres;

	std::vector<Vertex_PCU> m_UIVertices;
};