#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Collision2D.hpp"

class Clock;
class Timer;
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
	void StartUp();

	static double s_fixedDeltaTime;
	// Update each frame
	void Update( float deltaSeconds );
	void AdvanceSimulation( float deltaSeconds );
	void ApplyEffectors( float deltaSeconds );
	void ApplyAccelerationToRigidbody( Rigidbody2D* rb, float deltaSeconds );
	void MoveRigidbodies( float deltaSeconds );
	void CleanupDestroyedObjects();
	void ModifyGravity( float deltaGravity );
	void UpdateFrameStartPos();

	// Rigidbody
	Rigidbody2D* CreateRigidbody( Vec2 worldPos = Vec2::ZERO );
	void AddRigidbodyToList( Rigidbody2D* rb );
	void DestroyRigidbody( Rigidbody2D* rb );

	// Collider
	void AddColliderToList( Collider2D* collider );
	void DestroyCollider( Collider2D* collider );
	Collider2D* CreateCollider( Collider2DType type, Rigidbody2D* rb = nullptr );


	// DiscCollider2D
	DiscCollider2D* CreateDiscCollider( Vec2 worldPosition, float radius, Rigidbody2D* rb = nullptr );

	// PolygonCollider2D
	PolygonCollider2D* CreatePolyCollider( std::vector<Vec2> points, Rigidbody2D* rb = nullptr );
	PolygonCollider2D* CreatePolyCollider( Polygon2 polygon, Rigidbody2D* rb = nullptr );

	// Collision
	void DetectCollisions();	// A04
	void CollisionResponse();	// A04
	void ResolveCollisions();
	void ResolveCollision( const Collision2D& collision );
	void CorrectObjectsInCollision( const Collision2D& collision );
	float CalculateCollisionNormalImpulse( const Collision2D& collision );
	float CalculateCollisionTangentImpulse( const Collision2D& collision, float normalImpulse );
	void ApplyImpulseInCollision( const Collision2D& collision, Vec2 impulse );
	void CreateCollision( Collider2D* colA, Collider2D* colB, Manifold2D manifold );

	// Physic Time
	double GetTimeScale();
	double GetFixedDeltaTime() const { return Physics2D::s_fixedDeltaTime; }
	bool IsClockPause();
	void SetFixedDeltaTime( float frameTimeSeconds );
	void PausePhysicsTime();
	void ResumePhysicsTime();
	void SetTimeScale( double scale );

public:
	Vec2 m_gravityAccel = Vec2( 0.f, -2.f );

private:
	std::vector<Collider2D*> m_colliders;
	std::vector<Rigidbody2D*> m_rigidbodies;
	std::vector<Collision2D> m_collisions;
	Clock* m_clock = nullptr;
	Timer* m_timer = nullptr;
};
