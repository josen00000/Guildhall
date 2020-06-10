#include "Collider2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Algorithm.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Game/Game.hpp"

extern Game* g_theGame;

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

void Collider2D::InitDelegate()
{
// 	SubscribeEnterEvent( g_theGame, &Game::OnCollliderEnter );
// 	SubscribeStayEvent( g_theGame, &Game::OnCollliderStay );
// 	SubscribeLeaveEvent( g_theGame, &Game::OnCollliderLeave );
// 	SubscribeTriggerEnterEvent( g_theGame, &Game::OnTriggerEnter );
// 	SubscribeTriggerStayEvent( g_theGame, &Game::OnTriggerStay );
// 	SubscribeTriggerLeaveEvent( g_theGame, &Game::OnTriggerLeave );
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
		std::string debugString;
		if( manifold.contact.GetStartPos() == manifold.contact.GetEndPos() ) {
			debugString = std::string( "polygon Intersect" + std::to_string( result ) + " 111111 ");
		}
		else {
			debugString = std::string( "polygon Intersect" + std::to_string( result ) + " 222222 ");
		}
		DebugAddScreenText( Vec4( 0.f, 1.f, 10.f, -20.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 0.f, debugString ); 
		DebugAddWorldPoint( Vec3( manifold.contact.GetStartPos(), 0.f ), 10.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, DEBUG_RENDER_ALWAYS );
		DebugAddWorldPoint( Vec3( manifold.contact.GetEndPos(), 0.f ), 10.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, DEBUG_RENDER_ALWAYS );
		Vec3 arrowStart = ( manifold.contact.GetStartPos() + manifold.contact.GetEndPos() ) / 2.f;

		DebugAddWorldArrow( arrowStart, arrowStart + manifold.normal * 10.f, Rgba8::RED, 0.f, DEBUG_RENDER_ALWAYS );
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
	const PolygonCollider2D* polyColA = dynamic_cast<const PolygonCollider2D*>(colA);
	const PolygonCollider2D* polyColB = dynamic_cast<const PolygonCollider2D*>(colB);

	// Mid phase check
	Disc2 polyDiscA = polyColA->GetWorldBounds();
	Disc2 polyDiscB = polyColB->GetWorldBounds();
	if( !polyDiscA.IsIntersectWith( polyDiscB ) ){
		return false;
	}
	
	std::vector<Vec2> shapeA;
	std::vector<Vec2> shapeB;
	polyColA->m_worldPolygon.GetAllVertices( shapeA );
	polyColB->m_worldPolygon.GetAllVertices( shapeB );

	if( GJKIntersectCheck2D( Vec2( 1, 0 ), shapeA, shapeB ) ) {
		return true;
	}
	else {
		return false;
	}
}

Manifold2D GetDiscVSDiscManifold( const Collider2D* colA, const Collider2D* colB ) // colA is me, colB is other
{
	const DiscCollider2D* discColA = dynamic_cast<const DiscCollider2D*>( colA );
	const DiscCollider2D* discColB = dynamic_cast<const DiscCollider2D*>( colB );
	Manifold2D result;
	Vec2 disp = discColA->m_worldPosition - discColB->m_worldPosition;
	result.dist = discColB->m_radius + discColA->m_radius - disp.GetLength();
	result.normal = disp.GetNormalized();
	result.contact.m_start = discColA->m_worldPosition - result.normal * discColA->m_radius;
	result.contact.m_end = discColA->m_worldPosition - result.normal * discColA->m_radius;
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
	result.contact.m_start = discColA->m_worldPosition - result.normal * discColA->m_radius;
	result.contact.m_end = discColA->m_worldPosition - result.normal * discColA->m_radius;
	return result;
}

Manifold2D GetPolyVSPolyManifold( const Collider2D* colA, const Collider2D* colB ) // push a out of b
{
	const PolygonCollider2D* polyColA = dynamic_cast<const PolygonCollider2D*>(colA);
	const PolygonCollider2D* polyColB = dynamic_cast<const PolygonCollider2D*>(colB);


	std::vector<Vec2> shapeA;
	std::vector<Vec2> shapeB;
	std::vector<Vec2> simplex;
	Manifold2D result;
	polyColA->m_worldPolygon.GetAllVertices( shapeA );
	polyColB->m_worldPolygon.GetAllVertices( shapeB );

	GJKIntersectCheck2DAndGetSimplex( Vec2( 1.f, 0.f ), shapeA, shapeB, simplex );

	Vec2 penetration = GetGJK2DManifold( simplex, shapeA, shapeB );
	result.dist = penetration.GetLength();
	result.normal = -penetration.GetNormalized();

	// calculate the contact
	std::vector<Vec2> contacts;
	Vec2 supportB = GetGJK2DSupportPointOfShape( result.normal, shapeB );
	Plane2 rPlane = Plane2( result.normal, supportB );
	Vec2 tangent = result.normal.GetRotated90Degrees();

	for( int i = 0; i < shapeB.size(); i++ ) {
		Vec2 point = shapeB[i];
		if( rPlane.GetDistanceFromPlane( point ) < 0.1f ){
			contacts.push_back( point );
		}
	}

	// draw normal
	Vec2 tangentMin = contacts[0];
	Vec2 tangentMax = contacts[0];

 	if( contacts.size() == 1 ) {
		result.contact.m_start = tangentMin;
 		result.contact.m_end = tangentMin;
		if( polyColA->m_worldPolygon.GetDistanceToEdge( tangentMin ) >= 0.2f ) {
			//int test = 1;
		}
 		return result;
  	}
	FloatRange referenceRange = FloatRange(); 
	Vec2 planeOriginal = rPlane.GetplaneOriginalPoint();

	// calculate reference range
	for( int i = 0; i < contacts.size(); i++ ) {
		Vec2 point = contacts[i];
		Vec2 OP = point - planeOriginal;
		float length = GetProjectedLength2D( OP, tangent );
		if( i == 0 ) {
			referenceRange.Set( length, length );
			tangentMin = point;
			tangentMax = point;
		}
		if( length > referenceRange.maximum ){
			tangentMax = point;
		}
		if( length < referenceRange.minimum ) {
			tangentMin = point;
		}
		referenceRange.AppendFloatRange( length );

	}

	LineSegment2 referenceSeg = LineSegment2( tangentMin, tangentMax );

	Vec2 contactMin = Vec2::ZERO;
	Vec2 contactMax = Vec2::ZERO;
	FloatRange contactRange = FloatRange( referenceRange.minimum - 1.f );

	// get contact point
	for( int i = 0; i < polyColA->m_worldPolygon.GetEdgeCount(); i++ ) {
		LineSegment2 checkSeg = polyColA->m_worldPolygon.GetEdgeInWorld( i );
		LineSegment2 clippedSeg = LineSegment2::ClipSegmentToSegment( checkSeg, referenceSeg );

		if( clippedSeg.GetStartPos() == clippedSeg.GetEndPos() ){ continue; }
		Vec2 point = clippedSeg.GetStartPos();

		if( rPlane.IsPointInBack( point ) ) {
			Vec2 OP = point - planeOriginal;
			float length = GetProjectedLength2D( OP, tangent );
			if( contactRange.minimum == contactRange.maximum && contactRange.minimum == (referenceRange.minimum - 1.f) ) {
				contactRange.Set( length, length );
				contactMax = point;
				contactMin = point;
			}
			if( length > contactRange.maximum ) {
				contactMax = point;
			}
			if( length < contactRange.minimum ) {
				contactMin = point;
			}
			contactRange.AppendFloatRange( length );
		}

		point = clippedSeg.GetEndPos();

		if( rPlane.IsPointInBack( point ) ) {
			Vec2 OP = point - planeOriginal;
			float length = GetProjectedLength2D( OP, tangent );
			if( contactRange.minimum == contactRange.maximum && contactRange.minimum == (referenceRange.minimum - 1.f) ) {
				contactRange.Set( length, length );
				contactMin = point;
				contactMax = point;
			}
			if( length > contactRange.maximum ) {
				contactMax = point;
			}
			if( length < contactRange.minimum ) {
				contactMin = point;
			}
			contactRange.AppendFloatRange( length );
		}
	}
	result.contact.m_start = contactMin;
	result.contact.m_end = contactMax;

	if( polyColA->m_worldPolygon.GetDistanceToEdge( contactMin ) >= 0.2f || polyColA->m_worldPolygon.GetDistanceToEdge( contactMax ) >= 0.2f ){
		//int test = 1;
	}

	return  result;
}

Manifold2D GetPolyVSDiscManifold( const Collider2D* colA, const Collider2D* colB ){ // A is poly B is disc push a out of b
	Manifold2D result = GetDiscVSPolyManifold( colB, colA );
	result.contact.m_start = result.contact.m_start + result.normal * result.dist;
	result.contact.m_end = result.contact.m_start + result.normal * result.dist;
	result.normal = -result.normal;
	return result;
}