#include "RigidBody2D.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Physics/Collider2D.hpp"

Rigidbody2D::Rigidbody2D( Physics2D* owner, Collider2D* col /*= nullptr */ )
	:m_system(owner)
	,m_collider(col)
{
	m_worldPosition = Vec2::ZERO;
}

Rigidbody2D::~Rigidbody2D()
{
	// Assert the collider is already null
	// TODO: Check valid
}

void Rigidbody2D::Destroy()
{
	
	m_isDestroied = true;
	m_collider->Destroy();
}

void Rigidbody2D::SetCollider( Collider2D* collider )
{
	if( nullptr != m_collider ) {
		m_collider->Destroy();
	}

	m_collider = collider;
}

void Rigidbody2D::SetPosition( Vec2 position )
{
	m_worldPosition = position;
	m_collider->SetPosition( position );
}

void Rigidbody2D::DebugRenderCollider2D( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& filledColor )
{
	m_collider->DebugRender( ctx, borderColor, filledColor );
}

