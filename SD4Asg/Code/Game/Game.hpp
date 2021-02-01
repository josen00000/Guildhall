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
	void Render() const;
	void RenderUI() const;
	void CheckIfExit();
	void HandleInput();
	void HandleKeyboardInput();
	void HandleMouseInput();


	// object
	void RandomGenerateNewObject( AABB2 bounds );
	void HalveObjects();
	void DoubleObjects();
	void RotateObjectAtPoint( GameObject* obj, RotateDirection dirt, Vec2 point );

	// Accessor
	Vec2 GetMousePos() const { return m_mousePos; }
	GameObject* GetSelectedObject() const { return m_selectedObj; }

	// Mutator
	void SetSelectedObject( GameObject* obj );

private:
	void Update( float deltaSeconds );
	void UpdateMouse( float deltaSeconds );
	void UpdateDraggedObject();
	void StartDrag();
	void GenerateGameObjects();

	
	//Load
	void LoadAssets();


public:
	bool m_isAppQuit		= false;
	bool m_isDragging		= false;
	Vec2 m_mousePos			= Vec2::ZERO;
	GameObject* m_selectedObj = nullptr;
	std::vector<Vec2> m_draggedOffsets;

	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;

	RandomNumberGenerator* m_rng = nullptr;
	std::vector<GameObject*> m_objs;
};
