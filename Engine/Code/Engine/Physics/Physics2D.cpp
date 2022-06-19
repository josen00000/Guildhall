#include "Physics2D.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Core/Time/Timer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"

double Physics2D::s_fixedDeltaTime = 1.0 / 120;

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
	m_clock->SelfBeginFrame();
}

void Physics2D::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	while( m_timer->CheckAndDecrement() ){
		UpdateFrameStartPos();
		AdvanceSimulation( (float)GetFixedDeltaTime() );
	}
}

void Physics2D::AdvanceSimulation( float deltaSeconds )
{
	ApplyEffectors( deltaSeconds );
	MoveRigidbodies( deltaSeconds );
	RotateRigidbodies( deltaSeconds );
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
	
		// temporary for A02
	Vec2 accel = rb->m_force / rb->m_mass;
	accel += m_gravityAccel;

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

void Physics2D::RotateRigidbodies( float deltaSeconds )
{
	for( int i = 0 ; i < m_rigidbodies.size(); i++ ){
		if( m_rigidbodies[i] == nullptr ){ continue; }
		m_rigidbodies[i]->UpdateAngular( deltaSeconds );
	}
}

void Physics2D::DetectCollisions()
{
	m_lastFrameCollisions = m_curtFrameCollisions;
	m_curtFrameCollisions.clear();
	for( int colIndex = 0; colIndex < m_colliders.size(); colIndex++ ) {
		Collider2D* col1 = m_colliders[colIndex];
		if( col1 == nullptr || !col1->GetRigidbody()->IsEnablePhysics() ){
			continue;
		}
		for( int colIndex1 = colIndex + 1; colIndex1 < m_colliders.size(); colIndex1++ ) {
			Collider2D* col2 = m_colliders[colIndex1];
			if( col2 == nullptr || !col2->GetRigidbody()->IsEnablePhysics() ){
				continue;
			}
			if( !IsRigidBodysAbleInteract( col1->GetRigidbody(), col2->GetRigidbody() ) ){ 
				continue; }

			Manifold2D manifold;
			if( col1->IntersectsAndGetManifold( col2, manifold ) ){
				Collision2D tempColls = CreateCollision( col1, col2, manifold );
			}
		}
	}
	SendCollisionMessage();
}

void Physics2D::CollisionResponse()
{
	
}

void Physics2D::ResolveCollisions()
{
	for( int colIndex = 0; colIndex < m_curtFrameCollisions.size(); colIndex++ ){
		Collision2D& tempCol = m_curtFrameCollisions[colIndex];
		ResolveCollision( tempCol );
	}
}

void Physics2D::ResolveCollision( const Collision2D& collision )
{
	if( !collision.m_needResolve ) { return; }

	CorrectObjectsInCollision( collision );
	float nImpulse = CalculateCollisionNormalImpulse( collision );
	Vec2 normalImpulse = nImpulse * collision.GetNormal();
	float tImpulse = CalculateCollisionTangentImpulse( collision, nImpulse );
	Vec2 tangentImpulse = tImpulse * collision.GetTangent();
	ApplyImpulseInCollision( collision, normalImpulse );
	ApplyImpulseInCollision( collision, tangentImpulse );
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

float Physics2D::CalculateCollisionNormalImpulse( const Collision2D& collision )
{
	Rigidbody2D* rbMe = collision.me->m_rigidbody;
	Rigidbody2D* rbOther = collision.other->m_rigidbody;
	float result = 0;
	float res = PhysicsMaterial::GetRestitutionBetweenTwoMaterial( collision.me->m_material, collision.other->m_material );
	Vec2 velMe = rbMe->GetImpactVelocity( collision.GetContactCenter() );
	Vec2 velOther = rbOther->GetImpactVelocity( collision.GetContactCenter() );
	Vec2 normal = collision.GetNormal();
	Vec2 vAB = velOther - velMe;
	Vec2 contact = collision.GetContactCenter();
	Vec2 rAPTangent = contact - collision.me->GetCentroid();
	Vec2 rBPTangent = contact - collision.other->GetCentroid();
	rAPTangent.Rotate90Degrees();
	rBPTangent.Rotate90Degrees();
	float rAPDotSqur = DotProduct2D( rAPTangent, normal );
	float rBPDotSqur = DotProduct2D( rBPTangent, normal );
	rAPDotSqur = rAPDotSqur * rAPDotSqur;
	rBPDotSqur = rBPDotSqur * rBPDotSqur;

	result = ( 1 + res ) * ( DotProduct2D( vAB, normal ) );
	if( rbMe->GetSimulationMode() == RIGIDBODY_DYNAMIC && rbOther->GetSimulationMode() == RIGIDBODY_DYNAMIC ){
		result = result /  ( ( ( 1 / rbMe->GetMass() ) + ( 1 / rbOther->GetMass() ) ) + ( rAPDotSqur / rbMe->GetMoment()) + ( rBPDotSqur / rbOther->GetMoment() ) );
	}
	else if( rbMe->GetSimulationMode() == RIGIDBODY_DYNAMIC ){
		result = result / ( ( 1 / rbMe->GetMass() ) + ( rAPDotSqur / rbMe->GetMoment() ) );
	}
	else if( rbOther->GetSimulationMode() == RIGIDBODY_DYNAMIC ){
		result = result / (( 1 / rbOther->GetMass() ) + ( rBPDotSqur / rbOther->GetMoment() ) );
	}
	return result;
}



float Physics2D::CalculateCollisionTangentImpulse( const Collision2D& collision, float normalImpulse )
{
	Rigidbody2D* rbMe = collision.me->m_rigidbody;
	Rigidbody2D* rbOther = collision.other->m_rigidbody;
	float result = 0;
	float res = PhysicsMaterial::GetRestitutionBetweenTwoMaterial( collision.me->m_material, collision.other->m_material );
	Vec2 velMe = rbMe->GetImpactVelocity( collision.GetContactCenter() );
	Vec2 velOther = rbOther->GetImpactVelocity( collision.GetContactCenter() );
	Vec2 normal = collision.GetNormal();
	normal.Rotate90Degrees();
	Vec2 vAB = velOther - velMe;
	Vec2 contact = collision.GetContactCenter();
	Vec2 rAPTangent = contact - collision.me->GetCentroid();
	Vec2 rBPTangent = contact - collision.other->GetCentroid();
	rAPTangent.Rotate90Degrees();
	rBPTangent.Rotate90Degrees();
	float rAPDotSqur = DotProduct2D( rAPTangent, normal );
	float rBPDotSqur = DotProduct2D( rBPTangent, normal );
	rAPDotSqur = rAPDotSqur * rAPDotSqur;
	rBPDotSqur = rBPDotSqur * rBPDotSqur;

	result = ( 1 + res ) * ( DotProduct2D( vAB, normal ) );
	if( rbMe->GetSimulationMode() == RIGIDBODY_DYNAMIC && rbOther->GetSimulationMode() == RIGIDBODY_DYNAMIC ) {
		result = result /  ( ( ( 1 / rbMe->GetMass() ) + ( 1 / rbOther->GetMass() ) ) + ( rAPDotSqur / rbMe->GetMoment()) + ( rBPDotSqur / rbOther->GetMoment() ) );
	}
	else if( rbMe->GetSimulationMode() == RIGIDBODY_DYNAMIC ) {
		result = result / ( ( 1 / rbMe->GetMass() ) + ( rAPDotSqur / rbMe->GetMoment() ) );
	}
	else if( rbOther->GetSimulationMode() == RIGIDBODY_DYNAMIC ) {
		result = result / (( 1 / rbOther->GetMass() ) + ( rBPDotSqur / rbOther->GetMoment() ) );
	}
	float friction = collision.me->GetFrictionWith( collision.other );
	float sign = (result > 0) ? 1.f : -1.f;
	if( abs( result ) > abs( friction * normalImpulse ) ) {
		result = sign * normalImpulse * friction;
	}
	return result;
}

void Physics2D::ApplyImpulseInCollision( const Collision2D& collision, Vec2 impulse )
{
	Collider2D* me = collision.me;
	Collider2D* other = collision.other;
	
	if( me->m_rigidbody->GetSimulationMode() == other->m_rigidbody->GetSimulationMode()	&& me->m_rigidbody->GetSimulationMode() == RIGIDBODY_DYNAMIC ){
		collision.me->m_rigidbody->ApplyImpulse( impulse, collision.GetContactCenter() );
		collision.other->m_rigidbody->ApplyImpulse( -impulse, collision.GetContactCenter() );
	}
	else if( other->m_rigidbody->GetSimulationMode() == RIGIDBODY_DYNAMIC ){
		other->m_rigidbody->ApplyImpulse( -impulse, collision.GetContactCenter() );
	}
	else if(  me->m_rigidbody->GetSimulationMode() == RIGIDBODY_DYNAMIC ){
		me->m_rigidbody->ApplyImpulse( impulse, collision.GetContactCenter() );
	}
}

Collision2D Physics2D::CreateCollision( Collider2D* colA, Collider2D* colB, Manifold2D manifold )
{
	Collision2D collision = Collision2D( colA, colB, manifold );
	if( colB->m_isTrigger || colA->m_isTrigger ) {
		collision.m_needResolve = false;
	}
	m_curtFrameCollisions.push_back( collision );
	return collision;
}

bool Physics2D::IsSameCollision( const Collision2D& colA, const Collision2D& colB ) const
{
	return ( ( colA.me == colB.me && colA.other == colB.other) || ( colA.me == colB.other && colB.me == colA.other) );
}

void Physics2D::SendCollisionMessage()
{
	for( int i = 0; i < m_curtFrameCollisions.size(); i++ ) {
		CollisionState collsState = COLLISION_ENTER;
		Collision2D& curtColls = m_curtFrameCollisions[i];

		for( int j = 0; j < m_lastFrameCollisions.size(); j++ ) {
			Collision2D& lastColls = m_lastFrameCollisions[j];
			if( IsSameCollision( lastColls, curtColls ) ) {
				collsState = COLLISION_STAY;
				break;
			}
		}
		InvokeCollisionDelegate( curtColls, collsState );
	}

	for( int i = 0; i < m_lastFrameCollisions.size(); i++ ) {
		CollisionState collsState = COLLISION_LEAVE;
		Collision2D& lastColls = m_lastFrameCollisions[i];

		for( int j = 0; j < m_curtFrameCollisions.size(); j++ ) {
			Collision2D& curtColls = m_lastFrameCollisions[j];
			if( IsSameCollision( lastColls, curtColls ) ) {
				collsState = COLLISION_ENTER;
				break;
			}
		}
		if( collsState == COLLISION_LEAVE ) {
			InvokeCollisionDelegate( lastColls, collsState );
		}
	}
}

void Physics2D::InvokeCollisionDelegate( const Collision2D& colls, CollisionState state )
{
	Collider2D* colMe = colls.me;
	Collider2D* colOther = colls.other;
	switch( state )
	{
	case COLLISION_ENTER:
		if( colMe->m_isTrigger ) {
			colMe->m_triggerEnterDelegate.invoke( colls );
		}
		else {
			colMe->m_enterDelegate.invoke( colls );
		}

		if( colOther->m_isTrigger ) {
			colOther->m_triggerEnterDelegate.invoke( colls );
		}
		else {
			colOther->m_enterDelegate.invoke( colls );
		}
		break;
	case COLLISION_STAY:
		if( colMe->m_isTrigger ) {
			colMe->m_triggerStayDelegate.invoke( colls );
		}
		else {
			colMe->m_stayDelegate.invoke( colls );
		}

		if( colOther->m_isTrigger ) {
			colOther->m_triggerStayDelegate.invoke( colls );
		}
		else {
			colOther->m_stayDelegate.invoke( colls );
		}
		break;
	case COLLISION_LEAVE:
		if( colMe->m_isTrigger ) {
			colMe->m_triggerLeaveDelegate.invoke( colls );
		}
		else {
			colMe->m_leaveDelegate.invoke( colls );
		}
		if( colOther->m_isTrigger ) {

		}
		colOther->m_leaveDelegate.invoke( colls );
		break;
	}
}

void Physics2D::EnableLayerInteraction( uint layerIdx0, uint layerIdx1 )
{
	uint layerIdx0Bit = 1 << layerIdx0;
	uint layerIdx1Bit = 1 << layerIdx1;
	m_layers[layerIdx0] = m_layers[layerIdx0] | layerIdx1Bit;
	m_layers[layerIdx1] = m_layers[layerIdx1] | layerIdx0Bit;
}

void Physics2D::DisableLayerInteraction( uint layerIdx0, uint layerIdx1 )
{
	uint layerIdx0Bit = 1 << layerIdx0;
	uint layerIdx1Bit = 1 << layerIdx1;
	m_layers[layerIdx0] = m_layers[layerIdx0] & ~layerIdx1Bit;
	m_layers[layerIdx1] = m_layers[layerIdx1] & ~layerIdx0Bit;
}

bool Physics2D::IsLayersInteract( uint layerIdx0, uint layerIdx1 ) const
{
	uint layerIdx0Bit = 1 << layerIdx0;
	return ( (m_layers[layerIdx1] & layerIdx0Bit) != 0 );
}

bool Physics2D::IsRigidBodysAbleInteract( const Rigidbody2D* rb1, const Rigidbody2D* rb2 ) const
{
	return IsLayersInteract( rb1->GetLayer(), rb2->GetLayer() );
}

double Physics2D::GetTimeScale()
{
	return m_clock->GetScale();
}

bool Physics2D::IsClockPause()
{
	return m_clock->IsPaused();
}

void Physics2D::SetFixedDeltaTime( float frameTimeSeconds )
{
	s_fixedDeltaTime = frameTimeSeconds;
}

void Physics2D::PausePhysicsTime()
{
	m_clock->Pause();
}

void Physics2D::ResumePhysicsTime()
{
	m_clock->Resume();
}

void Physics2D::SetTimeScale( double scale )
{
	m_clock->SetScale( scale );
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

void Physics2D::UpdateFrameStartPos()
{
	for( int i = 0; i < m_rigidbodies.size(); i++ ){
		if( m_rigidbodies[i] == nullptr ) { continue; }
		m_rigidbodies[i]->UpdateFrameStartPos();
	}
}

void Physics2D::EndFrame()
{
}

void Physics2D::StartUp()
{
	m_clock = new Clock();
	m_timer = new Timer();

	m_timer->SetSeconds( m_clock, s_fixedDeltaTime );
	InitialLayer();
}

void Physics2D::InitialLayer()
{
	for( int i = 0; i < 32; i++ ) {
		m_layers[i] = 1 << i;
	}
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
	tempPolyCol->m_localPolygon.SetPoints( points );
	tempPolyCol->m_worldPolygon = tempPolyCol->m_localPolygon;
	rb->SetSimulationMode( RIGIDBODY_STATIC );
	return tempPolyCol;
}

PolygonCollider2D* Physics2D::CreatePolyCollider( ConvexPoly2 polygon, Rigidbody2D* rb /*= nullptr */ )
{
	PolygonCollider2D* polyCol = dynamic_cast<PolygonCollider2D*> (CreateCollider( COLLIDER2D_POLYGON, rb ));
	polyCol->m_localPolygon = polygon;
	polyCol->m_worldPolygon = polygon;
	polyCol->m_worldPos = polygon.GetCenter();
	rb->SetSimulationMode( RIGIDBODY_STATIC );
	return polyCol;
}

PolygonCollider2D* Physics2D::CreatePolyCollider( AABB2 box, Rigidbody2D* rb /*= nullptr */ )
{
	std::vector<Vec2> boxPoints;
	boxPoints.resize( 4 );
	box.GetCornerPositions( boxPoints.data() );
	PolygonCollider2D* polyCol = CreatePolyCollider( boxPoints, rb );
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
