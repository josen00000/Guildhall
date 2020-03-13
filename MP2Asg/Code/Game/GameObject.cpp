#include "GameObject.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/Polygon2.hpp"

extern Game*			g_theGame;
extern Physics2D*		g_thePhysics;
extern RenderContext*	g_theRenderer;
extern Camera*			g_gameCamera;
extern DevConsole*		g_theConsole;

GameObject::GameObject( Vec2 pos, float radius )
{
	m_rb = g_thePhysics->CreateRigidbody( pos );
	m_rb->SetCollider( g_thePhysics->CreateDiscCollider( pos, radius, m_rb ) );
}

GameObject::GameObject(  Polygon2 poly )
{
	m_rb = g_thePhysics->CreateRigidbody( poly.m_center );
	PolygonCollider2D* polyCol = g_thePhysics->CreatePolyCollider( poly, m_rb );
	m_rb->SetCollider( polyCol );
}

GameObject::GameObject( std::vector<Vec2> points )
{
	Polygon2 tempPoly = Polygon2( points );
	m_rb = g_thePhysics->CreateRigidbody( tempPoly.m_center );
	PolygonCollider2D* polyCol = g_thePhysics->CreatePolyCollider( tempPoly, m_rb );
	if( !polyCol->m_worldPolygon.IsConvex() ) {
		GUARANTEE_OR_DIE( false, "Try to create a concave polygon!" );
	}
	m_rb->SetCollider( polyCol );
}

GameObject::~GameObject()
{
	m_rb->Destroy();
	m_rb = nullptr;
}

void GameObject::CheckIfMouseIn( Vec2 mousePos )
{
	Collider2D* col = m_rb->GetCollider();
	switch( col->m_type )
	{
		case COLLIDER2D_DISC: {
			DiscCollider2D* discCol = dynamic_cast<DiscCollider2D*>(col);
			if( discCol->Contains( mousePos ) ) {
				m_isMouseIn = true;
			}
			else {
				m_isMouseIn = false;
			}
			break;
		}
		case COLLIDER2D_POLYGON: {
			PolygonCollider2D* polyCol = dynamic_cast<PolygonCollider2D*>(col);
			if( polyCol->Contains( mousePos ) ) {
				m_isMouseIn = true;
			}
			else {
				m_isMouseIn = false;
			}
			break;
		}
		default:
			break;
	}
}

void GameObject::CheckIfOutCameraVertical( Camera* camera )
{
	Vec2 pos = GetPosition();
	Collider2D* col = m_rb->GetCollider();
	switch( col->m_type )
	{
		case COLLIDER2D_DISC: {
			DiscCollider2D* discCol = dynamic_cast<DiscCollider2D*>(col);
			pos.y -= discCol->m_radius;
			break;
		}
		case COLLIDER2D_POLYGON:{
			PolygonCollider2D* polyCol = dynamic_cast<PolygonCollider2D*>(col);
			pos += polyCol->m_worldPolygon.GetLowestPoint();
			break;
		}
		default:
			break;
	}
	if( pos.y < camera->GetOrthoMin().y ) {
		Vec2 vel = m_rb->GetVelocity();
		if( vel.y < 0 ){
			vel.y = - vel.y;
			m_rb->SetVelocity( vel );
		}
	}

}

void GameObject::CheckIfOutCameraHorizontal( Camera* camera )
{
	Vec2 pos = GetPosition();
	Collider2D* col = m_rb->GetCollider();
	float dist = 0;
	switch( col->m_type )
	{
		case COLLIDER2D_DISC: {
			DiscCollider2D* discCol = (DiscCollider2D*)col;
			dist = discCol->m_radius;
			break;
		}
		case COLLIDER2D_POLYGON: {
			PolygonCollider2D* polyCol = (PolygonCollider2D*)col;
			dist = polyCol->m_worldPolygon.GetLongestDistance();
			break;
		}
		default:
			break;
	}
	if( pos.x < camera->GetOrthoMin().x - dist ) {
		pos.x = camera->GetOrthoMax().x + dist;
	}
	else if( pos.x > camera->GetOrthoMax().x + dist ) {
		pos.x = camera->GetOrthoMin().x - dist;
	}
	SetPosition( pos );
}

void GameObject::UpdateColliderShape()
{
	m_rb->GetCollider()->UpdateWorldShape();
}

void GameObject::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	CheckIfMouseIn( g_theGame->m_mousePos );
	CheckIfOutCameraVertical( g_gameCamera );
	CheckIfOutCameraHorizontal( g_gameCamera );
	UpdateColliderShape();
}

void GameObject::UpdateAngular( float deltaSeconds )
{
	m_rb->UpdateAngular( deltaSeconds );
}

void GameObject::Render() const
{
	Collider2D* col = m_rb->GetCollider();
	Rgba8 borderColor = Rgba8::BLUE;
	Rgba8 FilledColor = Rgba8( 255, 255, 255, 128 );
	if( m_rb->GetSimulationMode() == RIGIDBODY_STATIC ) {
		borderColor = Rgba8( 128, 128, 128 );
	}
	if( m_rb->GetSimulationMode() == RIGIDBODY_KINEMATIC ) {
		borderColor = Rgba8( 255, 0, 255 );
	}
	if( m_rb->GetSimulationMode() == RIGIDBODY_DYNAMIC ) {
		borderColor = Rgba8::BLUE;
	}
	if( m_isMouseIn ) {
		borderColor = Rgba8::YELLOW;
	}
	if( m_isSelected ) {
		borderColor = Rgba8( 0, 100, 0 );
	}
	if( m_isIntersect ) {
		FilledColor = Rgba8( 255, 0, 0, 128 );
	}
	float bounciness = m_rb->GetCollider()->GetBounciness();
	int test = (int)RangeMapFloat( 0.f, 1.f, 0.f, 255.f, bounciness );
	FilledColor.a = (unsigned char)test;
	std::string testString = "Color alpha is " + std::to_string( FilledColor.a );
	std::string testString1 = "bounciness is " + std::to_string( bounciness );
	g_theConsole->PrintString( Rgba8::RED, testString );
	g_theConsole->PrintString( Rgba8::BLUE, testString1 );
	col->DebugRender( g_theRenderer, borderColor, FilledColor );
	m_rb->DebugRender( g_theRenderer );
}

Vec2 GameObject::GetPosition() const
{
	return m_rb->GetPosition();
}

// void GameObject::CheckIntersectWith( GameObject* other ) 
// {
// 	Collider2D* col = m_rb->GetCollider();
// 	if( col->Intersects( other->m_rb->GetCollider() ) ) {
// 		m_isIntersect = true;
// 		other->SetIntersect( true );
// 	}
// }

void GameObject::DisablePhysics() {
	m_rb->DisablePhysics();
}

void GameObject::EnablePhysics()
{
	m_rb->EnablePhysics();
}



void GameObject::SetVelocity( Vec2 vel )
{
	m_rb->SetVelocity( vel );
}

void GameObject::SetSimulateMode( SimulationMode mode )
{
	m_rb->SetSimulationMode( mode );
}

void GameObject::UpdateBounciness( float deltaBounce )
{
	m_rb->GetCollider()->UpdateMaterialBounciness( deltaBounce );
}

void GameObject::UpdateMass( float deltaMass )
{
	m_rb->UpdateMass( deltaMass );
}

void GameObject::UpdateFriction( float deltaFric )
{
	m_rb->GetCollider()->UpdateMaterialFriction( deltaFric );
}

void GameObject::UpdateDrag( float deltaDrag )
{
	m_rb->UpdateDrag( deltaDrag );
}

void GameObject::UpdateAngularVelocity( float deltaAngVel )
{
	m_rb->UpdateAngularVelocity( deltaAngVel );
}

void GameObject::ResetAngularVelocity()
{
	m_rb->ResetAngularVelocity();
}

void GameObject::UpdateRotationRadians( float deltaRadians )
{
	m_rb->UpdateRotationRadians( deltaRadians );
}

void GameObject::SetPosition( Vec2 pos )
{
	m_rb->SetPosition( pos );
}

void GameObject::SetIntersect( bool isIntersect )
{
	m_isIntersect = isIntersect;
}
