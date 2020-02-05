#include "Game/GameObject.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"

extern Physics2D* g_thePhysics;
extern RenderContext* g_theRenderer;

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
}

void GameObject::Render() const
{
	DiscCollider2D* tempDiscCol = (DiscCollider2D*)m_rb->GetCollider();
	tempDiscCol->DebugRender( g_theRenderer, Rgba8::BLUE, Rgba8::RED );

}

void GameObject::SetRigidbody( Rigidbody2D* rb )
{
	m_rb = rb;
}
