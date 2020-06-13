#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"

class Camera;
class World;
class GameObject;
class CubeSphere;
class GPUMesh;
class InputSystem;
class RenderContext;
class RandomNumberGenerator;
class Texture;
class ShaderState;
class Material;
class Shader;
class RenderBuffer;

struct Vertex_PCU;
struct Mat44;

enum GameState {
	GAME_STATE_NULL = -1,
	GAME_STATE_LOADING,
	GAME_STATE_ATTRACT,
	GAME_STATE_PLAYING,
	GAME_STATE_END,
	NUM_GAME_STATE
};

struct ColorData {
	Mat44 colorMat;
	float pow;
	Vec3 pad;
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
	void RenderGame() const;
	void RenderUI() const;
	void RenderBasis() const;

	// convension
	const Convention GetConvention() const{ return m_convension; }
	void SetConvention( Convention convention ); 
	
private:
	void Update( float deltaSeconds );
	void UpdateUI( float deltaSeconds );
	void UpdateCamera(float deltaSeconds );
	void UpdateLighting( float deltaSeconds );

	// render
	void RenderDebugInfo() const;
	void RenderFPSInfo() const;

	// input
	void HandleInput( float deltaSeconds );
	void HandleKeyboardInput( float deltaSeconds );
	void HandleCameraMovement( float deltaSeconds );
	void HandleAudioKeyboardInput();
	void PlayerTestSound();

	//Game State Check
	void CheckIfExit();
	void CheckGameStates();
	void CheckIfPause();
	void CheckIfDeveloped();
	void CheckIfNoClip();

	//Load
	void LoadAssets();
	void LoadDefinitions();
	void LoadMaps( const char* mapFolderPath );
	template<typename T>
	void LoadFileDefinition( T t, const char* path );

	// Mesh
	void CreateGameObjects();


public:
	bool m_debugMode		= false;
	bool m_noClip			= false;
	bool m_isPause			= false;
	bool m_debugCamera		= false;
	bool m_isAppQuit		= false;
	bool m_isAttractMode	= false;
	bool m_isUsingGrayEffect = false;
	bool m_isUsingBloomEffect = false;

	GameState m_gameState	= GAME_STATE_NULL;
	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;
	RandomNumberGenerator* m_rng = nullptr;
	
	GPUMesh* m_cubeMesh	= nullptr;
	GameObject* m_cubeObj = nullptr;
	GameObject* m_cubeObj1 = nullptr;
	GameObject* m_cubeObj2 = nullptr;
	
	std::vector<Vertex_PCU> m_vertices;
	std::vector<Vertex_PCU> m_UIVertices;
	// convension
	Convention m_convension;

	World* m_world = nullptr;

private:
	float m_DebugDeltaSeconds = 0.f;
};


template<typename T>
void Game::LoadFileDefinition( T t, const char* path )
{
	XmlDocument defFile;
	defFile.LoadFile( path );
	const XmlElement* mapRootElement = defFile.RootElement();
	t( *mapRootElement );
}
