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
	DetectCollisions();
	ResolveCollisions();
	CleanupDestroyedObjects();
}

void Physics2D::ApplyEffectors( float deltaSeconds )
{
	for( int rbIndex = 0; rbIndex < m_rigidbodies.size(); rbIndex++ ) {
		Rigidbody2D* rb = m_rigidbodies[rbIndex];
		if( rb == nullptr || rb->m_mode == RIGIDBODY_STATIC || rb->m_mode == RIGIDBODY_KINEMATIC ){ continue; }
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
		if( rb == nullptr ){ continue; }
		Vec2 deltaPos = rb->GetVelocity() * deltaSeconds;
		if( rb->m_isEnable ) {
			if( rb->m_mode != RIGIDBODY_STATIC ) {
				rb->UpdatePositionPerFrame( deltaPos );
			}
		}
	}
}

void Physics2D::DetectCollisions()
{
	m_collisions.clear();
	for( int colIndex = 0; colIndex < m_colliders.size(); colIndex++ ) {
		Collider2D* col1 = m_colliders[colIndex];
		if( col1 == nullptr ){
			continue;
		}
		for( int colIndex1 = colIndex + 1; colIndex1 < m_colliders.size(); colIndex1++ ) {
			Collider2D* col2 = m_colliders[colIndex1];
			if( col2 == nullptr ){
				continue;
			}
			Manifold2D manifold;
			if( col1->IntersectsAndGetManifold( col2, manifold ) ){
				CreateCollision( col1, col2, manifold );
			}
		}
	}
}

void Physics2D::CollisionResponse()
{
	
}

void Physics2D::ResolveCollisions()
{
	for( int colIndex = 0; colIndex < m_collisions.size(); colIndex++ ){
		Collision2D& tempCol = m_collisions[colIndex];
		ResolveCollision( tempCol );
	}
}

void Physics2D::ResolveCollision( const Collision2D& collision )
{
	CorrectObjectsInCollision( collision );
	Vec2 impulse = CalculateCollisionImpulse( collision );
	ApplyImpulseInCollision( collision, impulse );
	
	// Dynamic vs dynamic 
	// kinematic vs kinematic
	// static don't move
	// dynamic vs ( kinematic || static )  only push dynamic
	// kinematic vs static only push kinematic
}

void Physics2D::CorrectObjectsInCollision( const Collision2D& collision )
{
	Collider2D* me = collision.me;
	Collider2D* other = collision.other;
	float myMass = collision.me->GetMass();
	float otherMass = collision.other->GetMass();
	if( me->m_rigidbody->GetSimulationMode() == other->m_rigidbody->GetSimulationMode() ) {
		float pushMe = otherMass / ( myMass + otherMass );
		float pushOther = 1 - pushMe;
		collision.me->Move( pushMe * collision.GetNormal() * collision.GetDist() );
		collision.other->Move( -pushOther * collision.GetNormal() * collision.GetDist() );
	}
	else if( me->m_rigidbody->GetSimulationMode() == RIGIDBODY_STATIC ){
		collision.other->Move( -collision.GetNormal() * collision.GetDist() );
	}
	else if( other->m_rigidbody->GetSimulationMode() == RIGIDBODY_STATIC ) {
		collision.me->Move( collision.GetNormal() * collision.GetDist() );
	}
	else {
		if( me->m_rigidbody->GetSimulationMode() == RIGIDBODY_DYNAMIC ) {
			collision.me->Move( collision.GetNormal() * collision.GetDist() );
		}
		else {
			collision.other->Move( -collision.GetNormal() * collision.GetDist() );
		}
	}
}

Vec2 Physics2D::CalculateCollisionImpulse( const Collision2D& collision )
{
	Rigidbody2D* rbMe = collision.me->m_rigidbody;
	Rigidbody2D* rbOther = collision.other->m_rigidbody;
	float result;
	float res = PhysicsMaterial::GetRestitutionBetweenTwoMaterial( collision.me->m_material, collision.other->m_material );
	Vec2 velMe = rbMe->GetVelocity();
	Vec2 velOther = rbOther->GetVelocity();
	Vec2 normal = collision.GetNormal();
	result = ( ( rbMe->GetMass() * rbOther->GetMass() ) / ( rbMe->GetMass() + rbOther->GetMass() ) ) * ( 1 + res ) * DotProduct2D( ( velOther - velMe ), normal );
	return result * normal;
}

void Physics2D::ApplyImpulseInCollision( const Collision2D& collision, Vec2 impulse )
{
	Collider2D* me = collision.me;
	Collider2D* other = collision.other;
	
	if( me->m_rigidbody->GetSimulationMode() == other->m_rigidbody->GetSimulationMode()	 ){
		collision.me->m_rigidbody->ApplyImpulse( impulse, Vec2::ZERO );
		collision.other->m_rigidbody->ApplyImpulse( -impulse, Vec2::ZERO );
	}
	else if( me->m_rigidbody->GetSimulationMode() == RIGIDBODY_STATIC ){
		other->m_rigidbody->ApplyImpulse( -2 * impulse, Vec2::ZERO );
	}
	else if( other->m_rigidbody->GetSimulationMode() == RIGIDBODY_STATIC ){
		me->m_rigidbody->ApplyImpulse( 2 * impulse, Vec2::ZERO );
	}
	else{
		if( me->m_rigidbody->GetSimulationMode() == RIGIDBODY_DYNAMIC ){
			me->m_rigidbody->ApplyImpulse( 2 * impulse, Vec2::ZERO );
		}
		else{
			other->m_rigidbody->ApplyImpulse( -2 * impulse, Vec2::ZERO );
		}
	}


	
}

void Physics2D::CreateCollision( Collider2D* colA, Collider2D* colB, Manifold2D manifold )
{
	Collision2D collision = Collision2D( colA, colB, manifold );
	m_collisions.push_back( collision );
}

void Physics2D::CleanupDestroyedObjects()
{
	for( int rIndex = 0; rIndex < m_rigidbodies.size(); rIndex++ ) {
		Rigidbody2D* tempRb = m_rigidbodies[rIndex];
		if( nullptr == tempRb ) { continue; }

		if( tempRb->IsDestroied() ) {
			delete tempRb;
			m_rigidbodies[rIndex] = nullptr;
		}
	}

	for( int cIndex = 0; cIndex < m_colliders.size(); cIndex++ ) {
		Collider2D* tempCollider = m_colliders[cIndex];
		if( nullptr == tempCollider ) { continue; }

		if( tempCollider->IsDestroied() ) {
			delete tempCollider;
			m_colliders[cIndex] = nullptr;
			// TODO tempCollider = nullptr; assign pointer doesn't work
		}
	}
}

void Physics2D::ModifyGravity( float deltaGravity )
{
	m_gravityAccel.y += deltaGravity;
}

void Physics2D::EndFrame()
{
}

Rigidbody2D* Physics2D::CreateRigidbody( Vec2 worldPos /*= Vec2::ZERO */ )
{
	Rigidbody2D* tempRb = new Rigidbody2D( this, worldPos );
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

DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 worldPosition, float radius, Rigidbody2D* rb /*= nullptr */ )
{
	DiscCollider2D* tempDiscCol = dynamic_cast<DiscCollider2D*> ( CreateCollider( COLLIDER2D_DISC, rb ) );
	tempDiscCol->m_radius = radius;
	tempDiscCol->m_worldPosition = worldPosition;
	rb->SetSimulationMode( RIGIDBODY_DYNAMIC );
	return tempDiscCol;
}

PolygonCollider2D* Physics2D::CreatePolyCollider( std::vector<Vec2> points, Rigidbody2D* rb /*= nullptr */ )
{
	PolygonCollider2D* tempPolyCol = dynamic_cast<PolygonCollider2D*> ( CreateCollider( Collider2DType::COLLIDER2D_POLYGON, rb ) );
	tempPolyCol->m_polygon.SetEdgesFromPoints( points );
	rb->SetSimulationMode( RIGIDBODY_STATIC );
	return tempPolyCol;
}

PolygonCollider2D* Physics2D::CreatePolyCollider( Polygon2 polygon, Rigidbody2D* rb /*= nullptr */ )
{
	PolygonCollider2D* polyCol = dynamic_cast<PolygonCollider2D*> (CreateCollider( COLLIDER2D_POLYGON, rb ));
	polyCol->m_polygon = polygon;
	polyCol->m_worldPos = polygon.m_center;
	rb->SetSimulationMode( RIGIDBODY_STATIC );
	return polyCol;
}

void Physics2D::DestroyCollider( Collider2D* collider )
{
	collider->Destroy();
}

Collider2D* Physics2D::CreateCollider( Collider2DType type, Rigidbody2D* rb /*= nullptr */ )
{
	Collider2D* col = nullptr;
	switch( type )
	{
	case COLLIDER2D_DISC:				col = new DiscCollider2D();
		break;
	case COLLIDER2D_POLYGON:			col = new PolygonCollider2D();
		break;
	}
	col->m_system = this;
	col->m_rigidbody = rb;
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
