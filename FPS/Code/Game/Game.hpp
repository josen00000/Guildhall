#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/EventSystem.hpp"


class Camera;
class GameObject;
class CubeSphere;
class GPUMesh;
class InputSystem;
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
	void RenderLightObjects() const;
	void RenderUI() const;

	
private:
	void Update( float deltaSeconds );
	void UpdateUI( float deltaSeconds );
	void UpdateCamera(float deltaSeconds );
	void UpdateLighting( float deltaSeconds );
	void CreateShaderNames();
	void CreateDebugScreen();
	void BindCurrentShader() const;

	void HandleDevKeyboardInput( float deltaSeconds );
	void HandleDebugKeyboardInput( float deltaSeconds );
	void HandleLightKeyboardInput( float deltaSeconds );
	void HandleMouseInput( float deltaSeconds );
	void HandleCameraMovement( float deltaSeconds );

	// shader
	void UseNextShader();
	void UsePreviousShader();
		 

	//Game State Check
	void CheckIfExit();
	void CheckGameStates();
	void CheckIfPause();
	void CheckIfDeveloped();
	void CheckIfNoClip();

	//Load
	void LoadAssets();

	// Mesh
	void CreateTestObjects();
	void UpdateMeshes( float deltaSeconds );
	void UpdateSphereMeshes( float deltaSeconds );
	void CreateSphereObjects();
	void CreateCubeSphereObjects();
	void RenderSpheres() const;
	void RenderTesSpheres() const;
	void RenderCubeSphere() const;

	// light objects
	void CreateLightQuadObjects();
	void CreateLightCubeObjects();
	void CreateLightSphereObjects();
	void UpdateLightObjects( float deltaSeconds );

	// light setting
	void SetAttenuation( Vec3 atten );
	void SetAttenuation( float x, float y, float z );

	// DebugRender
	void CreateDebugRenderObjects();

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
	
	GPUMesh* m_sphereMesh		= nullptr;
	GPUMesh* m_tesMesh			= nullptr;
	GPUMesh* m_cubeSphereMesh	= nullptr;
	
	// sd a 06
	GPUMesh* m_lightQuadMesh	= nullptr;
	GPUMesh* m_lightSphereMesh	= nullptr;
	GPUMesh* m_lightCubeMesh	= nullptr;

	GameObject* m_lightQuadObject	= nullptr;
	GameObject* m_lightSphereObject = nullptr;
	GameObject* m_lightCubeObject	= nullptr;
	
	GameObject* m_tesselationObject = nullptr;
	GameObject* m_cubeSphereObject = nullptr;

	std::vector<Vertex_PCU> m_vertices;
	std::vector<Vertex_PCU> m_UIVertices;
	std::vector<GameObject*> m_sphereObjects;
	
	std::vector<std::string> m_shaderNames;
	bool m_isLightFollowCamera = false;
	bool m_isLightFollowAnimation = false;
	int m_currentShaderIndex;
	float m_ambientLightIntensity = 0.f;
	Vec3 m_diffuseAttenuation = Vec3( 0.f, 1.f, 0.f );
	Vec3 m_lightPos;
	Vec3 m_specularAttenuation = Vec3( 0.f, 1.f, 0.f );
	Rgba8 m_ambientLightColor = Rgba8::GREEN;
	Rgba8 m_lightColor = Rgba8::RED;
	float m_lightIntensity = 1.f;
	float m_specularFactor = 1.f;
	float m_specularPow = 8.f;
};

// Light Command
bool LightCommandSetAmbientColor( EventArgs& args );
bool LightCommandSetLightColor( EventArgs& args );
