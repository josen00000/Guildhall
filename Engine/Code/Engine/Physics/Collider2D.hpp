#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Physics/PhysicsMaterial.hpp"
#include "Engine/Core/Delegate.hpp"

class Physics2D;
class RenderContext;
class Rigidbody2D;

struct Collision2D;
struct Disc2;
struct Rgba8;
struct Vec2;
struct Manifold2D;

enum Collider2DType {
	COLLIDER2D_DISC = 0,
	COLLIDER2D_POLYGON,
	NUM_COLLIDER
};

class Collider2D {
	friend class Physics2D;

protected:
	virtual ~Collider2D();

public:
	virtual void UpdateWorldShape() = 0;
	virtual void Destroy();
	virtual void InitDelegate();

	// queries
	virtual Vec2 GetClosestPoint( const Vec2& pos )		const = 0;
	virtual bool Contains( const Vec2& pos )			const = 0;
	bool Intersects( const Collider2D* other ) const;	
	bool IntersectsAndGetManifold( const Collider2D* other, Manifold2D& manifold );
	//Manifold2D GetManifold();
	float GetBounceWith( const Collider2D* other ) const;
	float GetFrictionWith( const Collider2D* other )const;
	void Move( Vec2 displacement );

	// Accessor
	virtual bool IsDestroied() const { return m_isDestroyed; }
	Collider2DType GetType() const { return m_type; }
	virtual Disc2 GetWorldBounds() const = 0;
	Rigidbody2D* GetRigidbody() const { return m_rigidbody; }
	virtual Vec2 GetCentroid() const = 0;
	float GetMass() const; 

	// Mutator
	virtual void SetPosition( Vec2 pos );
	void EnableTrigger(){ m_isTrigger = true; }
	void DissableTrigger(){ m_isTrigger = false; }

	// Bounciness
	void UpdateMaterialBounciness( float deltaRes );
	void UpdateMaterialFriction( float deltaFric );
	float GetBounciness() const;

	// friction
	float GetFriction() const;

	// collision
	template<typename OBJ_TYPE>
	void SubscribeEnterEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) );
	template<typename OBJ_TYPE>
	void SubscribeStayEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) );
	template<typename OBJ_TYPE>
	void SubscribeLeaveEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) );
	template<typename OBJ_TYPE>
	void SubscribeTriggerEnterEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) );
	template<typename OBJ_TYPE>
	void SubscribeTriggerStayEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) );
	template<typename OBJ_TYPE>
	void SubscribeTriggerLeaveEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) );


	// Angular
	virtual float CalculateMoment( float mass ) const = 0;

	// debug helpers
	virtual void DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor ) = 0;

public:
	Collider2DType	m_type;

protected:
	bool			m_isDestroyed	= false;
	bool			m_isTrigger		= false;	
	Physics2D*		m_system		= nullptr;
	Rigidbody2D*	m_rigidbody		= nullptr;
	PhysicsMaterial m_material;


private:
	Delegate<Collision2D> m_enterDelegate;
	Delegate<Collision2D> m_stayDelegate; 
	Delegate<Collision2D> m_leaveDelegate;
	Delegate<Collision2D> m_triggerEnterDelegate;
	Delegate<Collision2D> m_triggerStayDelegate;
	Delegate<Collision2D> m_triggerLeaveDelegate;
};

template<typename OBJ_TYPE>
void Collider2D::SubscribeEnterEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) )
{
	m_enterDelegate.subscribe_method( obj, mcb );
}

template<typename OBJ_TYPE>
void Collider2D::SubscribeStayEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) )
{
	m_leaveDelegate.subscribe_method( obj, mcb );
}

template<typename OBJ_TYPE>
void Collider2D::SubscribeLeaveEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) )
{
	m_leaveDelegate.subscribe_method( obj, mcb );
}

template<typename OBJ_TYPE>
void Collider2D::SubscribeTriggerEnterEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) )
{
	m_triggerEnterDelegate.subscribe_method( obj, mcb );
}

template<typename OBJ_TYPE>
void Collider2D::SubscribeTriggerStayEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) )
{
	m_triggerStayDelegate.subscribe_method( obj, mcb );
}

template<typename OBJ_TYPE>
void Collider2D::SubscribeTriggerLeaveEvent( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(Collision2D) )
{
	m_triggerLeaveDelegate.subscribe_method( obj, mcb );
}

