#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Physics/PhysicsMaterial.hpp"

class Physics2D;
class RenderContext;
class Rigidbody2D;
class Collider2D;

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

	// queries
	virtual Vec2 GetClosestPoint( const Vec2& pos )		const = 0;
	virtual bool Contains( const Vec2& pos )			const = 0;
	bool Intersects( const Collider2D* other ) const;	
	bool IntersectsAndGetManifold( const Collider2D* other, Manifold2D& manifold );
	Manifold2D GetManifold();
	float GetBounceWith( const Collider2D* other ) const;
	void Move( Vec2 displacement );

	// Accessor
	virtual bool IsDestroied() const { return m_isDestroyed; }
	Collider2DType GetType() const { return m_type; }
	virtual Disc2 GetWorldBounds() const = 0;
	float GetMass() const; 

	// Mutator
	virtual void SetPosition( Vec2 pos );

	// Bounciness
	void UpdateMaterialBounciness( float deltaRes );
	void UpdateMaterialFriction( float deltaFric );
	float GetBounciness() const;

	// debug helpers
	virtual void DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor ) = 0;

public:
	Collider2DType	m_type;

protected:
	bool			m_isDestroyed	= false;
	Physics2D*		m_system		= nullptr;
	Rigidbody2D*	m_rigidbody		= nullptr;
	PhysicsMaterial m_material;
};

