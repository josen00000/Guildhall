#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Physics/RaycastResult2.hpp"
#include "Engine/Math/SpacePartition/SymmetricQuadTree.hpp"

class RenderContext;
class InputSystem;
class RandomNumberGenerator;
class Camera;
class GameObject;
class World;

enum RaycastStrategy
{
	NO_INVISIBLE_RAYCAST,
	NO_BROAD_CHECK,
	IMPLICIT_QUAD_TREE,
	NUM_OF_STRATEGY
};

struct Vertex_PCU;

struct RaycastTest {
	Vec2 startPos = Vec2::ZERO;
	Vec2 fwdDirt = Vec2::ZERO;
	float dist = 0.f;
	RaycastResult2 result;
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
	void UpdateDraggedObject();

	// Accessor
	Vec2 GetMousePos() const { return m_mousePos; }
	GameObject* GetSelectedObject() const { return m_selectedObj; }
	std::vector<GameObject*> GetPossibleObjsWithImplicitQuadTree( Vec2 pos, Vec2 fwdDirt, float dist );
	std::string GetRaycastStrategyString() const;
	// Mutator
	void SetSelectedObject( GameObject* obj );

private:
	void Update( float deltaSeconds );
	void UpdateMouse( );
	void RestartGame();
	// Object
	void RenderObjects() const;

	// raycast
	void GenerateInvisibleRaycast();
	void UpdateVisibleRaycast();
	void UpdateInvisibleRaycast();
	void StartSingleRaycast();
	void EndSingleRaycast();
	void RenderRaycast() const;
	void DebugRenderRaycast() const;
	void StartDrag();
	void GenerateGameObjects();

	
	// BSP & SP
	void ConstructSPTree();
	void ConstructSPQuadTreeWithFourPlanes( SymmetricQuadTree<GameObject*>* node, Plane2 bottom, Plane2 top, Plane2 left, Plane2 right, std::vector<GameObject*> content, int depth );
	void DebugRenderSPTree( SymmetricQuadTree<GameObject*>* node ) const;

	//Load
	void LoadAssets();


public:
	RaycastStrategy m_strategy		= NO_INVISIBLE_RAYCAST;
	bool m_isAppQuit				= false;
	bool m_isDebug					= false;
	bool m_isDragging				= false;
	bool m_isRaycast				= false;
	bool m_isPaused					= false;

	int hitNum1						= 0;
	int hitNum2						= 0;
	float m_deltaSeconds			= 0.f;
	float m_debugTime				= 0.f;

	Vec2 m_mousePos					= Vec2::ZERO;
	Vec2 m_testRaycastStartPoint	= Vec2::ZERO;
	RaycastResult2 m_testRaycastResult;
	GameObject* m_selectedObj = nullptr;
	std::vector<Vec2> m_draggedOffsets;

	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;

	RandomNumberGenerator* m_rng = nullptr;
	std::vector<GameObject*> m_objs;
	std::vector<Vertex_PCU> m_points;
	std::vector<uint> m_indexes;
	std::vector<Vec2> m_raycastStartPoints;
	
	std::vector<RaycastTest> m_invisibleRaycast;

	
	// SP Tree Structure
	SymmetricQuadTree<GameObject*>* m_root;

	// debug data
	Vec2 m_debugIntersectPoint;

};
