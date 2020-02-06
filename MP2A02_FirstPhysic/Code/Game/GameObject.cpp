#include "Game/GameObject.hpp"
#include "Game/Game.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"

extern Game*			g_theGame;
extern Physics2D*		g_thePhysics;
extern RenderContext*	g_theRenderer;

GameObject::GameObject( Vec2 pos, float radius )
{
	// Create rigidbody
	// Create collider
	m_rb = g_thePhysics->CreateRigidbody();
	DiscCollider2D* tempDiscCol = g_thePhysics->CreateDiscCollider( pos, radius );
	m_rb->SetCollider( tempDiscCol );
	m_pos = pos;
	m_radius = radius;
}

GameObject::~GameObject()
{
	m_rb->Destroy();
	m_rb = nullptr;
}

void GameObject::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	CheckIfMouseIn( g_theGame->m_mousePos );
}

void GameObject::Render() const
{
	DiscCollider2D* tempDiscCol = (DiscCollider2D*)m_rb->GetCollider();
	Rgba8 borderColor = Rgba8::BLUE;
	Rgba8 FilledColor = Rgba8( 255, 255, 255, 128 );
	if( m_isMouseIn ) {
		borderColor = Rgba8::YELLOW;	
	}
	if( m_isSelected ) {
		borderColor = Rgba8( 0, 100, 0 );
	}
	if( m_isTouching ) {
		FilledColor = Rgba8( 255, 0, 0, 128 );
	}
	
	tempDiscCol->DebugRender( g_theRenderer, borderColor, FilledColor );

}

void GameObject::CheckIfMouseIn( Vec2 mousePos )
{
	DiscCollider2D* DiscCol = (DiscCollider2D*)m_rb->GetCollider();
	if( DiscCol->Contains( mousePos ) ) {
		m_isMouseIn = true;
	}
	else {
		m_isMouseIn = false;
	}
}

bool GameObject::IsIntersectWith( GameObject* obj ) const
{
	if( m_rb->GetCollider()->Intersects( obj->m_rb->GetCollider() ) ) {
		return true;
	}
	else {
		return false;
	}
}

void GameObject::SetRigidbody( Rigidbody2D* rb )
{
	m_rb = rb;
}

void GameObject::SetPosition( Vec2 pos )
{
	m_pos = pos;
	m_rb->SetPosition( pos );

}

void GameObject::SetIsSelected( bool selected )
{
	m_isSelected = selected;
}

void GameObject::SetIsTouching( bool touching )
{
	m_isTouching = touching;
}
