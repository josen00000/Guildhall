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
	bool IsDestroied() const { return m_isDestroyed; }
	bool IsEnablePhysics() const { return m_isEnable; }
	float GetMass() const { return m_mass; }
	float GetDrag() const { return m_drag; }
	float GetRotationInRadians() const { return m_rotationInRadians; }
	float GetAngularVelocity() const { return m_angularVelocity; }
	float GetFrameTorque() const { return m_frameTorque; }
	float GetMoment() const { return m_moment; }
	Vec2 GetImpactVelocity( Vec2 contact );
	Vec2 GetPosition() const { return m_worldPosition; }
	Vec2 GetVelocity() const { return m_velocity; }
	Vec2 GetVerletVelocity() const;
	Collider2D* GetCollider() const { return m_collider; }
	SimulationMode GetSimulationMode() const { return m_mode; }
	
	// Mutator
	void SetMass( float mass );
	void SetRotationInRadians( float rot );
	void SetAngularVelocity( float angVel ); 
	void SetFrameTorque( float frameTorque ); 
	void SetMoment( float moment );
	void SetPosition( Vec2 position );
	void SetVelocity( Vec2 velocity );
	void SetCollider( Collider2D* collider ); 
	void SetColliderPosition();
	void SetSimulationMode( SimulationMode mode );

	void UpdateDrag( float deltaDrag );
	void UpdateVelocityPerFrame( const Vec2& deltaVel );
	void UpdatePositionPerFrame( const Vec2& deltaPos );
	void UpdateMass( float deltaMass );
	void UpdateFrameStartPos();

	// rotation
	void UpdateAngularVelocity( float deltaAngVel );
	void ResetAngularVelocity();
	void UpdateRotationRadians( float deltaRadians );
	void UpdateAngular( float deltaSeconds );

	void DisablePhysics();
	void EnablePhysics();

	void ApplyImpulse( Vec2 impulse, Vec2 point );
	void ApplyDragForce();

	void AddForce( Vec2 force );

	// help
	void DebugRenderCollider2D( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& filledColor );
	void DebugRender( RenderContext* ctx );

private:
	bool m_isDestroyed	= false;
	bool m_isEnable		= true;
	float m_mass		= 1.f;
	float m_drag		= 0.f;
	float m_rotationInRadians	= 0.f;
	float m_angularVelocity		= 0.f;
	float m_frameTorque			= 0.f;
	float m_moment				= 0.f;
	Vec2 m_velocity		= Vec2::ZERO;
	Vec2 m_force		= Vec2::ZERO;
	Vec2 m_frameStartPosition = Vec2::ZERO;
	Vec2 m_worldPosition;

	SimulationMode m_mode = RIGIDBODY_DYNAMIC;
	Physics2D*	m_system	= nullptr;
	Collider2D* m_collider	= nullptr;

};