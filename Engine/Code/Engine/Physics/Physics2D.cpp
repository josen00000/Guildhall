#include "Physics2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"

Physics2D::~Physics2D()
{
	for( int rIndex = 0; rIndex < m_rigidbodies.size(); rIndex++ ) {
		Rigidbody2D* tempRb = m_rigidbodies[rIndex];
		if( nullptr != tempRb ){ continue; }
		
		delete tempRb;
	}

	for( int cIndex = 0; cIndex < m_colliders.size(); cIndex++ ) {
		Collider2D* tempCollider = m_colliders[cIndex];
		if( nullptr != tempCollider ){ continue; }

		delete tempCollider;
	}
}

void Physics2D::BeginFrame()
{

}

void Physics2D::Update( float deltaSeconds )
{
	AdvanceSimulation( deltaSeconds );
}

void Physics2D::AdvanceSimulation( float deltaSeconds )
{
	ApplyEffectors( deltaSeconds );
	MoveRigidbodies( deltaSeconds );
	CleanupDestroyedObjects();
}

void Physics2D::ApplyEffectors( float deltaSeconds )
{
	for( int rbIndex = 0; rbIndex < m_rigidbodies.size(); rbIndex++ ) {
		Rigidbody2D* rb = m_rigidbodies[rbIndex];
		ApplyAccelerationToRigidbody( rb, deltaSeconds );		
	}
}

void Physics2D::ApplyAccelerationToRigidbody( Rigidbody2D* rb, float deltaSeconds )
{
	// apply acceleration
	Vec2 accel = m_gravityAccel;	// temporary for A02

	// Calculate velocity
	Vec2 deltaVel = accel * deltaSeconds;
	if( rb->m_isEnable ) {
		rb->UpdateVelocityPerFrame( deltaVel );
	}
}

void Physics2D::MoveRigidbodies( float deltaSeconds )
{
	for( int rbIndex = 0; rbIndex < m_rigidbodies.size(); rbIndex++ ) {
		Rigidbody2D* rb = m_rigidbodies[rbIndex];
		Vec2 deltaPos = rb->GetVelocity() * deltaSeconds;
		if( rb->m_isEnable ) {
			rb->UpdatePositionPerFrame( deltaPos );
		}
	}
}

void Physics2D::DetectCollisions()
{

}

void Physics2D::CollisionResponse()
{
	
}

void Physics2D::CleanupDestroyedObjects()
{
	for( int rIndex = 0; rIndex < m_rigidbodies.size(); rIndex++ ) {
		Rigidbody2D* tempRb = m_rigidbodies[rIndex];
		if( nullptr == tempRb ) { continue; }

		if( tempRb->IsDestroied() ) {
			delete tempRb;
			tempRb = nullptr;
		}
	}

	for( int cIndex = 0; cIndex < m_colliders.size(); cIndex++ ) {
		Collider2D* tempCollider = m_colliders[cIndex];
		if( nullptr == tempCollider ) { continue; }

		if( tempCollider->IsDestroied() ) {
			delete tempCollider;
			tempCollider = nullptr;
		}
	}

}

void Physics2D::EndFrame()
{
}

Rigidbody2D* Physics2D::CreateRigidbody()
{
	Rigidbody2D* tempRb = new Rigidbody2D( this );
	AddRigidbodyToList( tempRb );
	return tempRb;
}

void Physics2D::AddRigidbodyToList( Rigidbody2D* rb )
{
	for( int rIndex = 0; rIndex < m_rigidbodies.size(); rIndex++ ) {
		if( m_rigidbodies[rIndex] == nullptr ) {
			m_rigidbodies[rIndex] = rb;
			return;
		}
	}

	m_rigidbodies.push_back( rb );
}

void Physics2D::DestroyRigidbody( Rigidbody2D* rb )
{
	rb->Destroy();
}

DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 worldPosition, float radius )
{
	DiscCollider2D* tempDiscCol = dynamic_cast<DiscCollider2D*> ( CreateCollider( COLLIDER2D_DISC ) );
	tempDiscCol->m_radius = radius;
	tempDiscCol->m_worldPosition = worldPosition;
	return tempDiscCol;
}

PolygonCollider2D* Physics2D::CreatePolyCollider( const Vec2* points, int pointCount )
{
	PolygonCollider2D* tempPolyCol = dynamic_cast<PolygonCollider2D*> ( CreateCollider( Collider2DType::COLLIDER2D_POLYGON ) );
	tempPolyCol->m_polygon.SetEdgesFromPoints( points, pointCount );
	return tempPolyCol;
}

PolygonCollider2D* Physics2D::CreatePolyCollider( Polygon2 polygon )
{
	PolygonCollider2D* polyCol = dynamic_cast<PolygonCollider2D*> ( CreateCollider( COLLIDER2D_POLYGON) );
	polyCol->m_polygon = polygon;
	return polyCol;
}

void Physics2D::DestroyCollider( Collider2D* collider )
{
	collider->Destroy();
}


Collider2D* Physics2D::CreateCollider( Collider2DType type )
{
	Collider2D* col = nullptr;
	switch( type )
	{
	case COLLIDER2D_DISC:				col = new DiscCollider2D();
		break;
	case COLLIDER2D_POLYGON:			col = new PolygonCollider2D();
		break;
	}
	AddColliderToList( col );
	return col;
}

void Physics2D::AddColliderToList( Collider2D* collider )
{
	for( int cIndex = 0; cIndex < m_colliders.size(); cIndex++ ) {
		if( nullptr == m_colliders[cIndex] ) {
			m_colliders[cIndex] = collider;
			return;
		}
	}

	m_colliders.push_back( collider );
}
