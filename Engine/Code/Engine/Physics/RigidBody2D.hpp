#pragma once
#include "Engine/Math/vec2.hpp"

class Collider2D;
class Physics2D;
class RenderContext;
struct Rgba8;

enum SimulationMode {
	RIGIDBODY_STATIC,		// Don't move, apply effector
	RIGIDBODY_DYNAMIC,		// Move, apply effector
	RIGIDBODY_KINEMATIC		// Move
};

class Rigidbody2D {
	friend class Physics2D;

public:
	Rigidbody2D( Physics2D* owner, Vec2 worldPos, Collider2D* col = nullptr  );

private:
	~Rigidbody2D();

public:
	void Destroy();
	
	void Move( Vec2 displacement );


	// Accessor
	float GetMass() const { return m_mass; }
	bool IsDestroied() const { return m_isDestroyed; }
	Vec2 GetPosition() const { return m_worldPosition; }
	Vec2 GetVelocity() const { return m_velocity; }
	Collider2D* GetCollider() const { return m_collider; }
	SimulationMode GetSimulationMode() const { return m_mode; }
	
	// Mutator
	void SetPosition( Vec2 position );
	void SetVelocity( Vec2 velocity );
	void SetCollider( Collider2D* collider ); 
	void SetColliderPosition();
	void SetMass( float mass );
	void SetSimulationMode( SimulationMode mode );
	void UpdateDrag( float deltaDrag );
	void UpdateVelocityPerFrame( const Vec2& deltaVel );
	void UpdatePositionPerFrame( const Vec2& deltaPos );
	void UpdateMass( float deltaMass );

	void DisablePhysics();
	void EnablePhysics();

	void ApplyImpulse( Vec2 impulse, Vec2 point );
	// help
	void DebugRenderCollider2D( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& filledColor );
	void DebugRender( RenderContext* ctx );

private:
	bool m_isDestroyed	= false;
	bool m_isEnable		= true;
	float m_mass		= 1.f;
	float m_drag		= 0.f;
	Vec2 m_velocity		= Vec2::ZERO;

	SimulationMode m_mode = RIGIDBODY_DYNAMIC;
	Physics2D*	m_system	= nullptr;
	Collider2D* m_collider	= nullptr;

	Vec2 m_worldPosition;
};