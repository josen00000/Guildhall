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
	void GenerateTempPoints();
	void GenerateTestPoints();

private:
	void Update( float deltaSeconds );
	void UpdatePhysics( float deltaSeconds );
	void UpdateUI( float deltaSeconds );


	//Game State Check
	void CheckIfExit();

	// Camera
	void UpdateCamera( float deltaSeconds );
	void UpdateCameraHeight( float deltaSeconds );
	void UpdateCameraPos( float deltaSeconds );
	void SetCameraToOrigin();

	// GetInput
	void UpdateMouse();
	void UpdateMouseWheel();
	void UpdateMousePos();
	void HandleMouseInput();
	void HandleKeyboardInput();
	
	// Draw Polygon
	void BeginDrawPolygon();
	void EndDrawPolygon();
	void CancelDrawPolygon();
	bool IsPointEnoughForDrawPolygon() const;
	bool IsDrawedPolygonConvex() const;
	void AddPointToDrawPolygon( Vec2 point );
	void RenderDrawingPolygon() const;

	// Gameobject
	bool SelectGameObject();

	//Load
	void LoadAssets();


public:
	bool m_isAppQuit		= false;
	bool m_isDrawMode		= false;
	bool m_isDragging		= false;

	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;

	RandomNumberGenerator* m_rng = nullptr;

	Vec2 m_mousePos;
	float m_cameraMoveSpeed = 10.f;
	Vec2 m_cameraMoveDirct;
	float m_mouseScroll;

	Polygon2 m_drawPoly;
	GameObject* m_selectedObj;
	Vec2 m_selectOffset;

	std::vector<GameObject*> m_gameObjects;
	std::vector<Vec2> m_testPoints;	// Can be delete later
	std::vector<Vec2> m_tempPoints;	// Can be delete later
	std::vector<Vec2> m_drawPoints;
	

};
