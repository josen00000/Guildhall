#include "GameObject.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"

extern RenderContext* g_theRenderer;

GameObject::GameObject( Vec2 pos, float radius )
	:m_position(pos)
{
	m_rb = g_thePhysics->CreateRigidbody();
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
	m_position = centerPos;
}

void GameObject::Update( float deltaSeconds )
{

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
	col->DebugRender( g_theRenderer, Rgba8::RED, Rgba8::WHITE );

}
