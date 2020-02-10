#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Game/GameObject.hpp"

class DiscCollider2D;
class PolygonCollider2D;
class Rigidbody2D;
struct Polygon2;

class Physics2D {
public:
	~Physics2D();

public:
	void BeginFrame();
	void EndFrame(); // Cleanup destroyed objects

	// Update each frame
	void Update( float deltaSeconds );
	void AdvanceSimulation( float deltaSeconds );
	void ApplyEffectors( float deltaSeconds );
	void ApplyAccelerationToRigidbody( Rigidbody2D* rb, float deltaSeconds );
	void MoveRigidbodies( float deltaSeconds );
	void DetectCollisions();	// A04
	void CollisionResponse();	// A04
	void CleanupDestroyedObjects();
	

	// Rigidbody
	Rigidbody2D* CreateRigidbody();
	void AddRigidbodyToList( Rigidbody2D* rb );
	void DestroyRigidbody( Rigidbody2D* rb );

	// Collider
	void AddColliderToList( Collider2D* collider );
	void DestroyCollider( Collider2D* collider );
	Collider2D* CreateCollider( Collider2DType type );

	// DiscCollider2D
	DiscCollider2D* CreateDiscCollider( Vec2 worldPosition, float radius );
	// PolygonCollider2D
	PolygonCollider2D* CreatePolyCollider( const Vec2* points, int pointCount );
	PolygonCollider2D* CreatePolyCollider( Polygon2 polygon );
public:
	Vec2 m_gravityAccel = Vec2( 0.f, -0.01f );

private:
	std::vector<Collider2D*> m_colliders;
	std::vector<Rigidbody2D*> m_rigidbodies;
};
