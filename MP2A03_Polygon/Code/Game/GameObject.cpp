#include "GameObject.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern Game*			g_theGame;
extern Physics2D*		g_thePhysics;
extern RenderContext*	g_theRenderer;

GameObject::GameObject( Vec2 pos, float radius )
{
	m_rb = g_thePhysics->CreateRigidbody( pos );
	m_rb->SetCollider( g_thePhysics->CreateDiscCollider( pos, radius ) );
}

GameObject::GameObject(  Polygon2 poly )
{
	m_rb = g_thePhysics->CreateRigidbody();
	PolygonCollider2D* polyCol = g_thePhysics->CreatePolyCollider( poly );
	m_rb->SetCollider( polyCol );
	SetAverageCenterByPoints();
}

GameObject::GameObject( std::vector<Vec2> points )
{
	m_rb = g_thePhysics->CreateRigidbody();
	PolygonCollider2D* polyCol = g_thePhysics->CreatePolyCollider( points );
	if( !polyCol->m_polygon.IsConvex() ) {
		GUARANTEE_OR_DIE( false, "Try to create a concave polygon!" );
	}
	m_rb->SetCollider( polyCol );
	SetAverageCenterByPoints();
}

GameObject::~GameObject()
{
	m_rb->Destroy();
	m_rb = nullptr;
}

void GameObject::SetAverageCenterByPoints()
{
	PolygonCollider2D* tempPolyCol = (PolygonCollider2D*)m_rb->GetCollider();
	Polygon2 tempPoly = tempPolyCol->m_polygon; 
	Vec2 centerPos = Vec2::ZERO;
	for( int edgeIndex = 0; edgeIndex < tempPoly.GetEdgeCount(); edgeIndex++ ) {
		centerPos += tempPoly.GetEdge( edgeIndex ).GetStartPos();
	}
	centerPos /= (float)tempPoly.GetEdgeCount();
}

void GameObject::CheckIfMouseIn( Vec2 mousePos )
{
	DiscCollider2D* DiscCol = dynamic_cast<DiscCollider2D*>( m_rb->GetCollider() );
	if( DiscCol->Contains( mousePos ) ) {
		m_isMouseIn = true;
	}
	else {
		m_isMouseIn = false;
	}
}

void GameObject::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	CheckIfMouseIn( g_theGame->m_mousePos );
}

void GameObject::Render() const
{
	Collider2D* col = m_rb->GetCollider();
	Rgba8 borderColor = Rgba8::BLUE;
	Rgba8 FilledColor = Rgba8( 255, 255, 255, 128 );
	if( m_isMouseIn ) {
		borderColor = Rgba8::YELLOW;
	}
	if( m_isSelected ) {
		borderColor = Rgba8( 0, 100, 0 );
	}
	if( m_isIntersect ) {
		FilledColor = Rgba8( 255, 0, 0, 128 );
	}
	col->DebugRender( g_theRenderer, borderColor, FilledColor );
	m_rb->DebugRender( g_theRenderer );
}

Vec2 GameObject::GetPosition() const
{
	return m_rb->GetPosition();
}

void GameObject::CheckIntersectWith( GameObject* other ) 
{
	Collider2D* col = m_rb->GetCollider();
	Collider2D* otherCol = other->m_rb->GetCollider();
	if( col->Intersects( other->m_rb->GetCollider() ) ) {
		m_isIntersect = true;
		other->SetIntersect( true );
	}
	else {
		m_isIntersect = false;
		other->SetIntersect( false );
	}
}

void GameObject::DisablePhysics() {
	m_rb->DisablePhysics();
}

void GameObject::EnablePhysics()
{
	m_rb->EnablePhysics();
}

void GameObject::SetPosition( Vec2 pos )
{
	m_rb->SetPosition( pos );
}

void GameObject::SetIntersect( bool isIntersect )
{
	m_isIntersect = isIntersect;
}
