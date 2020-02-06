#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Engine/Math/vec2.hpp"

class Camera;
class GameObject;
class InputSystem;
class RandomNumberGenerator;
class RenderContext;

struct Vertex_PCU;

class Game {
public:
	Game(){}
	~Game(){}
	explicit Game( Camera* gameCamera );

	//basic
	void Startup();
	void Shutdown();
	void RunFrame( float deltaSeclnds );
	void EndFrame();
	void Reset();
	void Render() const;
	void RenderGameObjects() const;

private:
	void Update( float deltaSeconds );

	// GameObject
	bool SelectGameObject();
	void UpdateGameObjects( float deltaSeconds );
	void CreateGameObject();
	void DeleteGameObject( GameObject* obj );
	void UpdateGameObjectsTouching();

	// Camera
	void UpdateCamera( float deltaSeconds );
	void UpdateCameraHeight( float deltaSeconds );
	void UpdateCameraPos( float deltaSeconds );
	void SetCameraToOrigin();

	//Game State Check
	void CheckIfExit();
	
	// Mouse
	void UpdateMouse( float deltaSeconds );
	void UpdateMouseWheel();
	void UpdateMousePos();
	void RenderMouse() const;

public:
	bool m_isAppQuit		= false;
	bool m_isDragging		= false;
	float m_cameraSpeed = 5.f;
	float m_mouseScroll;
	Vec2 m_mousePos;
	Vec2 m_cameraDirection;

	Camera* m_gameCamera			= nullptr;
	RandomNumberGenerator* m_rng	= nullptr;
	
	std::vector<Vertex_PCU> m_UIVertices;
	
	std::vector<GameObject*> m_gameObjects;
	GameObject* m_selectedObj;
	Vec2 m_selectOffset;
};
