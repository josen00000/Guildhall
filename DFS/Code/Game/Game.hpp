#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Polygon2.hpp"

class RenderContext;
class InputSystem;
class RandomNumberGenerator;
class Camera;
class GameObject;
class World;

struct Vertex_PCU;

class Game {
public:
	Game(){}
	~Game(){}
	explicit Game( Camera* gameCamera, Camera* UICamera );

	//basic
	void Startup();
	void Shutdown();
	void RunFrame( float deltaSeclnds );
	void RenderGame() const;
	void RenderUI() const;

	// game object
	void DeleteGameObject( GameObject* obj );
	void CleanDestroyedObjects();

private:
	void UpdateGame( float deltaSeconds );
	void UpdateUI( float deltaSeconds );

	//Game State Check
	void CheckIfExit();

	// GetInput
	void HandleKeyboardInput();

	//Load
	void LoadAssets();
	void LoadDefs();

public:
	bool m_isAppQuit		= false;

	World* m_world			= nullptr;

	RandomNumberGenerator* m_rng = nullptr;

	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;


	std::vector<GameObject*> m_gameObjects;
};
