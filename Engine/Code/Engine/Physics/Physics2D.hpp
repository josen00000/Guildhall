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
struct ConvexPoly2;
struct AABB2;

enum CollisionState {
	COLLISION_ENTER,
	COLLISION_STAY,
	COLLISION_LEAVE,
};


class Physics2D {
public:
	~Physics2D();

public:
	void BeginFrame();
	void EndFrame(); // Cleanup destroyed objects
	void StartUp();
	void InitialLayer();

	static double s_fixedDeltaTime;
	// Update each frame
	void Update( float deltaSeconds );
	void AdvanceSimulation( float deltaSeconds );
	void ApplyEffectors( float deltaSeconds );
	void ApplyAccelerationToRigidbody( Rigidbody2D* rb, float deltaSeconds );
	void MoveRigidbodies( float deltaSeconds );
	void RotateRigidbodies( float deltaSeconds );
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
	PolygonCollider2D* CreatePolyCollider( ConvexPoly2 polygon, Rigidbody2D* rb = nullptr );
	PolygonCollider2D* CreatePolyCollider( AABB2 box, Rigidbody2D* rb = nullptr );


	// Collision
	void DetectCollisions();	// A04
	void CollisionResponse();	// A04
	void ResolveCollisions();
	void ResolveCollision( const Collision2D& collision );
	void CorrectObjectsInCollision( const Collision2D& collision );
	float CalculateCollisionNormalImpulse( const Collision2D& collision );
	float CalculateCollisionTangentImpulse( const Collision2D& collision, float normalImpulse );
	void ApplyImpulseInCollision( const Collision2D& collision, Vec2 impulse );
	Collision2D CreateCollision( Collider2D* colA, Collider2D* colB, Manifold2D manifold );
	bool IsSameCollision( const Collision2D& colA, const Collision2D& colB ) const;
	void SendCollisionMessage();
	void InvokeCollisionDelegate( const Collision2D& colls, CollisionState state );
	
	// layer
	void EnableLayerInteraction( uint layerIdx0, uint layerIdx1 );
	void DisableLayerInteraction( uint layerIdx0, uint layerIdx1 );
	bool IsLayersInteract( uint layerIdx0, uint layerIdx1 ) const;
	bool IsRigidBodysAbleInteract( const Rigidbody2D* rb1, const Rigidbody2D* rb2 ) const;

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
	Clock* m_clock = nullptr;

private:
	std::vector<Collider2D*> m_colliders;
	std::vector<Rigidbody2D*> m_rigidbodies;
	std::vector<Collision2D> m_curtFrameCollisions;
	std::vector<Collision2D> m_lastFrameCollisions;
	Timer* m_timer = nullptr;
	uint m_layers[32] = {0};
};
