#include "Collider2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/Collision2D.hpp"

typedef bool ( *collision_check_cb )( const Collider2D*, const Collider2D* );
typedef Manifold2D ( *calculate_manifold )( const Collider2D*, const Collider2D* );

static Manifold2D GetDiscVSDiscManifold( const Collider2D* colA, const Collider2D* colB );
static Manifold2D GetDiscVSPolyManifold( const Collider2D* colA, const Collider2D* colB );
static Manifold2D GetPolyVSDiscManifold( const Collider2D* colA, const Collider2D* colB );
static Manifold2D GetPolyVSPolyManifold( const Collider2D* colA, const Collider2D* colB );

static bool DiscVSDiscCollisionCheck( const Collider2D* colA, const Collider2D* colB );
static bool DiscVSPolygonCollisionCheck( const Collider2D* colA, const Collider2D* colB );
static bool PolygonVSPolygonCollisionCheck( const Collider2D* colA, const Collider2D* colB );

static	collision_check_cb CollisionMatrix[NUM_COLLIDER * NUM_COLLIDER] = {
	DiscVSDiscCollisionCheck,			nullptr,
	DiscVSPolygonCollisionCheck,		PolygonVSPolygonCollisionCheck
};

static calculate_manifold ManifoldMatrix[NUM_COLLIDER * NUM_COLLIDER] = {
	GetDiscVSDiscManifold,			GetDiscVSPolyManifold,
	GetPolyVSDiscManifold,			GetPolyVSPolyManifold
};

void Collider2D::Destroy()
{
	m_isDestroyed = true;
}

bool Collider2D::Intersects( const Collider2D* other ) const
{
	Collider2DType myType = m_type;
	Collider2DType otherType = other->m_type;
	Disc2 selfBound = GetWorldBounds();
	Disc2 otherBound = other->GetWorldBounds();
	if( !selfBound.IsIntersectWith( otherBound ) ){ return false; }

	int index;
	if( myType <= otherType ){
		index = otherType * NUM_COLLIDER + myType;
	}
	else {
		index = myType * NUM_COLLIDER + otherType;
	}
	collision_check_cb check = CollisionMatrix[index];
	return check( this, other );
}

bool Collider2D::IntersectsAndGetManifold( const Collider2D* other, Manifold2D& manifold )
{
	Collider2DType myType = m_type;
	Collider2DType otherType = other->m_type;
	Disc2 selfBound = GetWorldBounds();
	Disc2 otherBound = other->GetWorldBounds();
	if( !selfBound.IsIntersectWith( otherBound ) ) { return false; }

	int index;
	bool result;
	if( myType <= otherType ) {
		index = otherType * NUM_COLLIDER + myType;
		collision_check_cb check = CollisionMatrix[index];
		result = check( this, other );
	}
	else {
		index = myType * NUM_COLLIDER + otherType;
		collision_check_cb check = CollisionMatrix[index];
		result = check( other, this );
	}

	if( result ) {
		int mIndex = myType * NUM_COLLIDER + otherType;
		calculate_manifold getManifold = ManifoldMatrix[mIndex];
		manifold = getManifold( this, other );
		return true;
	}
	else {
		manifold.dist = 0.f;
		manifold.normal = Vec2::ZERO;
		return false;
	}
}

float Collider2D::GetBounceWith( const Collider2D* other ) const
{
	float result = PhysicsMaterial::GetRestitutionBetweenTwoMaterial( m_material, other->m_material );
	return result;
}

float Collider2D::GetFrictionWith( const Collider2D* other ) const
{
	float result = PhysicsMaterial::GetFrictionBetweenTwoMaterial( m_material, other->m_material );
	return result;
}

void Collider2D::Move( Vec2 displacement )
{
	m_rigidbody->Move( displacement );
}

float Collider2D::GetMass() const
{
	 return m_rigidbody->GetMass(); 
}

void Collider2D::SetPosition( Vec2 pos )
{
	UNUSED(pos);
}

void Collider2D::UpdateMaterialBounciness( float deltaRes )
{
	m_material.UpdateRestitution( deltaRes );
}

void Collider2D::UpdateMaterialFriction( float deltaFric )
{
	m_material.UpdateFriction( deltaFric );
}

float Collider2D::GetBounciness() const
{
	return m_material.m_restitution;
}

float Collider2D::GetFriction() const
{
	return m_material.m_friction;
}

Collider2D::~Collider2D()
{

}


// static function
bool DiscVSDiscCollisionCheck( const Collider2D* colA, const Collider2D* colB )
{
	const DiscCollider2D* discColA = dynamic_cast<const DiscCollider2D*>(colA);
	const DiscCollider2D* discColB = dynamic_cast<const DiscCollider2D*>(colB);
	Vec2 centerA = discColA->m_worldPosition;
	Vec2 centerB = discColB->m_worldPosition;
	Vec2 dispAB = centerA - centerB;
	float radiusA = discColA->m_radius;
	float radiusB = discColB->m_radius;
	float distSqr = dispAB.GetLengthSquared();
	if( distSqr <= (radiusA + radiusB) * (radiusB + radiusA) ) {
		return true;
	}
	else {
		return false;
	}
}

bool DiscVSPolygonCollisionCheck( const Collider2D* colA, const Collider2D* colB ) // A is the disc collider, B is polygon collider
{
	const DiscCollider2D* discColA = dynamic_cast<const DiscCollider2D*>(colA);
	const PolygonCollider2D* polyColB = dynamic_cast<const PolygonCollider2D*>(colB);

	// Mid phase check
	Disc2 polyDisc = polyColB->GetWorldBounds();
	if( !polyDisc.IsIntersectWith( discColA->m_worldPosition, discColA->m_radius ) ) {
		return false;
	}

	Vec2 closestPoint = polyColB->GetClosestPoint( discColA->m_worldPosition );
	float distSqr = GetDistanceSquared2D( discColA->m_worldPosition, closestPoint );
	if( distSqr > (discColA->m_radius * discColA->m_radius) ) {
		return false;
	}
	else {
		return true;
	}
	return true;
}

bool PolygonVSPolygonCollisionCheck( const Collider2D* colA, const Collider2D* colB )
{
	UNUSED( colB );
	UNUSED( colA );
	return true;
}

Manifold2D GetDiscVSDiscManifold( const Collider2D* colA, const Collider2D* colB ) // colA is me, colB is other
{
	const DiscCollider2D* discColA = dynamic_cast<const DiscCollider2D*>( colA );
	const DiscCollider2D* discColB = dynamic_cast<const DiscCollider2D*>( colB );
	Manifold2D result;
	Vec2 disp = discColA->m_worldPosition - discColB->m_worldPosition;
	result.dist = discColB->m_radius + discColA->m_radius - disp.GetLength();
	result.normal = disp.GetNormalized();
	result.contact = discColA->m_worldPosition - result.normal * discColA->m_radius;
	return result;
}

Manifold2D GetDiscVSPolyManifold( const Collider2D* colA, const Collider2D* colB ) // colA is Disc(me), colB is Poly(other)
{
	const DiscCollider2D* discColA = dynamic_cast<const DiscCollider2D*>( colA );
	const PolygonCollider2D* polyColB = dynamic_cast<const PolygonCollider2D*>( colB );
	Manifold2D result;
	Vec2 closestPoint = polyColB->GetClosestPoint( discColA->m_worldPosition );
	float dist = GetDistance2D( closestPoint, discColA->m_worldPosition );
	if( polyColB->Contains( discColA->m_worldPosition ) ){
		// Check on edge
		result.normal = ( closestPoint - discColA->m_worldPosition  ).GetNormalized();
		result.dist = dist + discColA->m_radius;
	}
	else{
		result.normal = ( discColA->m_worldPosition - closestPoint ).GetNormalized();
		result.dist	 = discColA->m_radius - dist;
	}
	result.contact = discColA->m_worldPosition - result.normal * discColA->m_radius;
	return result;
}

Manifold2D GetPolyVSPolyManifold( const Collider2D* colA, const Collider2D* colB )
{
	UNUSED(colB);
	UNUSED(colA);
	return Manifold2D();
}

Manifold2D GetPolyVSDiscManifold( const Collider2D* colA, const Collider2D* colB ){ // A is poly B is disc push a out of b
	Manifold2D result = GetDiscVSPolyManifold( colB, colA );
	result.contact = result.contact + result.normal * result.dist;
	result.normal = -result.normal;
	return result;
}