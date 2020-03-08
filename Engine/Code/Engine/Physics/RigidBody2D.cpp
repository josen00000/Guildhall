#include "RigidBody2D.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"


Rigidbody2D::Rigidbody2D( Physics2D* owner, Vec2 worldPos, Collider2D* col /*= nullptr */ )
	:m_system( owner )
	,m_worldPosition(worldPos)
	,m_collider(col)
{

}

Rigidbody2D::~Rigidbody2D()
{
	// Assert the collider is already null
	// TODO: Check valid
}

void Rigidbody2D::Destroy()
{
	//Physics2D::De
	m_isDestroyed = true;
	m_collider->Destroy();
}

void Rigidbody2D::Move( Vec2 displacement )
{
	SetPosition( m_worldPosition + displacement );
}

void Rigidbody2D::SetCollider( Collider2D* collider )
{
	if( nullptr != m_collider ) {
		m_collider->Destroy();
	}

	m_collider = collider;
}

void Rigidbody2D::SetColliderPosition()
{
	switch( m_collider->m_type )
	{
		case COLLIDER2D_DISC: {
			DiscCollider2D* discCol = (DiscCollider2D*)m_collider;
			discCol->SetPosition( m_worldPosition );
			break;
		}
		case COLLIDER2D_POLYGON: {
			PolygonCollider2D* polyCol = (PolygonCollider2D*) m_collider;
			polyCol->SetPosition( m_worldPosition );
		}
		default:
			break;
	}
}

void Rigidbody2D::SetMass( float mass )
{
	m_mass = mass;
}

void Rigidbody2D::SetSimulationMode( SimulationMode mode )
{
	m_mode = mode;
}

void Rigidbody2D::UpdateDrag( float deltaDrag )
{
	m_drag += deltaDrag;
	if( m_drag < 0 ){
		m_drag = 0;
	}
}

void Rigidbody2D::UpdateVelocityPerFrame( const Vec2& deltaVel )
{
	m_velocity += deltaVel;
}

void Rigidbody2D::UpdatePositionPerFrame( const Vec2& deltaPos )
{
	m_worldPosition += deltaPos;
	if( !m_isDestroyed ){
		SetColliderPosition();
	}
}

void Rigidbody2D::UpdateMass( float deltaMass )
{
	m_mass += deltaMass;
	if( m_mass < 0.001f ){
		m_mass = 0.001f;
	}
}

void Rigidbody2D::UpdateFrameStartPos()
{
	m_frameStartPosition = m_worldPosition;
}

void Rigidbody2D::DisablePhysics()
{
	m_isEnable = false;
}

void Rigidbody2D::EnablePhysics()
{
	m_isEnable = true;
}

void Rigidbody2D::ApplyImpulse( Vec2 impulse, Vec2 point )
{
	UNUSED(point);
	m_velocity += impulse / m_mass;
}

void Rigidbody2D::ApplyDragForce()
{
	Vec2 vel = GetVelocity();
	Vec2 dragForce = - vel * m_drag;
	AddForce( dragForce );
}

void Rigidbody2D::AddForce( Vec2 force )
{
	m_force += force;
}

Vec2 Rigidbody2D::GetVerletVelocity() const
{
	Vec2 verletVelocity = ( m_worldPosition - m_frameStartPosition ) / 1;
	return verletVelocity;
}

void Rigidbody2D::SetPosition( Vec2 position )
{
	m_worldPosition = position;
	m_collider->SetPosition( position );
}

void Rigidbody2D::SetVelocity( Vec2 velocity )
{
	m_velocity = velocity;
}

void Rigidbody2D::DebugRenderCollider2D( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& filledColor )
{
	m_collider->DebugRender( ctx, borderColor, filledColor );
}

void Rigidbody2D::DebugRender( RenderContext* ctx )
{
	Rgba8 renderColor;
	if( m_isEnable ) {
		renderColor = Rgba8::BLUE;
	}
	else {
		renderColor = Rgba8::RED;
	}
	Vec2 bottomLeft		= m_worldPosition + Vec2( -0.5f, -0.5f );
	Vec2 bottomRight	= m_worldPosition + Vec2( 0.5f, -0.5f );
	Vec2 topLeft		= m_worldPosition + Vec2( -0.5f, 0.5f );
	Vec2 topRight		= m_worldPosition + Vec2( 0.5f, 0.5f );
	ctx->DrawLine( bottomLeft, topRight, 0.2f, renderColor );
	ctx->DrawLine( topLeft, bottomRight, 0.2f, renderColor );
}

