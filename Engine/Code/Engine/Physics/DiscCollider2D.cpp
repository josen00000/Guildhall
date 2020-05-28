#include "DiscCollider2D.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Renderer/Context/RenderContext.hpp"


DiscCollider2D::DiscCollider2D( const Vec2& localPos, const Vec2& worldPos, float radius )
	:m_localPosition(localPos)
	,m_worldPosition(worldPos)
	,m_radius(radius)
{
	m_type = COLLIDER2D_DISC;
	InitDelegate();
}

DiscCollider2D::DiscCollider2D( const Vec2& worldPos, float radius )
	:m_worldPosition(worldPos)
	,m_radius(radius)
{
	m_type = COLLIDER2D_DISC;
	InitDelegate();
}

DiscCollider2D::DiscCollider2D()
{
	m_type = COLLIDER2D_DISC;
	InitDelegate();
}

DiscCollider2D::~DiscCollider2D()
{

}

void DiscCollider2D::UpdateWorldShape()
{

}

Vec2 DiscCollider2D::GetClosestPoint( const Vec2& pos ) const
{
	Vec2 result = GetNearestPointOnDisc2D( pos, m_worldPosition, m_radius );
	return result;
}

bool DiscCollider2D::Contains( const Vec2& pos ) const
{
	if( IsPointInDisc( pos, m_worldPosition, m_radius ) ) {
		return true;
	}
	else {
		return false;
	}
}

Disc2 DiscCollider2D::GetWorldBounds() const
{
	Disc2 result = Disc2( m_worldPosition, m_radius );
	return result;
}

Vec2 DiscCollider2D::GetCentroid() const
{
	return m_worldPosition;
}

// bool DiscCollider2D::Intersects( const Collider2D* other ) const
// {
// 	//TODO
// 	//DiscCollider2D* temp = dynamic_cast<DiscCollider2D*>( other);
// 	switch( other->GetType() )
// 	{
// 		case COLLIDER2D_DISC:{
// 			DiscCollider2D* discOther =(DiscCollider2D*)other;
// 			Vec2 disp = discOther->m_worldPosition - m_worldPosition;
// 			float distSq = disp.GetLengthSquared();
// 			if( distSq > (discOther->m_radius + m_radius) * (discOther->m_radius + m_radius) ) {
// 				return false;
// 			}
// 			else {
// 				return true;
// 			}
// 		}
// 		case COLLIDER2D_POLYGON:{
// 			PolygonCollider2D* polyOther = (PolygonCollider2D*)other;
// 			Vec2 closestPoint = polyOther->GetClosestPoint( m_worldPosition );
// 			float distSqr = GetDistanceSquared2D( closestPoint, m_worldPosition );
// 			if( distSqr > ( m_radius * m_radius ) ){
// 				return false;
// 			}
// 			else {
// 				return true;
// 			}
// 		}
// 
// 		default:
// 			return false;
// 	}
// 	
// }

void DiscCollider2D::SetPosition( Vec2 pos )
{
	m_worldPosition = pos;
}

float DiscCollider2D::CalculateMoment( float mass ) const
{
	return 0.5f * mass * m_radius * m_radius;
}

void DiscCollider2D::DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor )
{
	// TODO test and debug 
	float m_rotRadians = m_rigidbody->GetRotationInRadians();
	Vec2 direction = Vec2( 1.f, 0.f );
	direction.SetAngleRadians( m_rotRadians );
	ctx->DrawLine( m_worldPosition, ( m_worldPosition + direction * m_radius ), 0.5f, Rgba8::BLACK );
	ctx->DrawCircle( m_worldPosition, m_radius, 1, borderColor );
	ctx->DrawFilledCircle( m_worldPosition, ( m_radius - 0.9f ), fillColor );
}