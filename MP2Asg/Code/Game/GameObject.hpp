#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"

class Game;
class RandomNumberGenerator;
class Physics2D;
struct Rgba8;
struct Polygon2;

extern Physics2D* g_thePhysics;

class GameObject {
public:
	//constructor
	//GameObject( Vec2 pos, const Vec2* points, int pointCount );
	GameObject( Vec2 pos, float radius );
	GameObject( Polygon2 poly );
	GameObject( std::vector<Vec2> points );
	~GameObject();

private:
	void CheckIfMouseIn( Vec2 mousePos );
	void CheckIfOutWorldBoundVertical( Camera* camera );
	void CheckIfOutCameraHorizontal( Camera* camera );
	void UpdateColliderShape();

public:
	void Update( float deltaSeconds );
	void UpdateAngular( float deltaSeconds );
	virtual void Render() const;

	// Accessor
	Vec2 GetPosition()	const ;
	bool IsMouseIn()	const { return m_isMouseIn; }
	bool IsDestroyed()	const { return m_isDestroyed; }
	bool IsSelected()	const { return m_isSelected; }
	bool IsIntersect()	const { return m_isIntersect; }
	
	// Mutator
	void SetPosition( Vec2 pos );
	void SetIntersect( bool isIntersect );

	void DisablePhysics();
	void EnablePhysics();
	void SetVelocity( Vec2 vel );
	void SetSimulateMode( SimulationMode mode );

	void UpdateBounciness( float deltaBounce );
	void UpdateMass( float deltaMass ); 
	void UpdateFriction( float deltaFric );
	void UpdateDrag( float deltaDrag );
	
	// rotation
	void UpdateAngularVelocity( float deltaAngVel );
	void ResetAngularVelocity();
	void UpdateRotationRadians( float deltaRadians );

public:
	bool m_isDestroyed	= false;
	bool m_isMouseIn	= false;
	bool m_isSelected	= false;
	bool m_isIntersect	= false;
	Rigidbody2D* m_rb;
};

