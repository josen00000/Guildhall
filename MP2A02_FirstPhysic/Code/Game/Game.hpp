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

	//Game State Check
	void CheckIfExit();

	// Mouse
	void UpdateMousePos();
	void RenderMouse() const;

	// Game
	void CreateGameObject();
	void DeleteGameObject();
public:
	bool m_isAppQuit		= false;
	Vec2 m_mousePos;

	Camera* m_gameCamera			= nullptr;
	RandomNumberGenerator* m_rng	= nullptr;
	
	std::vector<Vertex_PCU> m_UIVertices;
	std::vector<GameObject*> m_gameObjects;

};
