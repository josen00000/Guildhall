#include "DiscCollider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

DiscCollider2D::DiscCollider2D( const Vec2& localPos, const Vec2& worldPos, float radius )
	:m_localPosition(localPos)
	,m_worldPosition(worldPos)
	,m_radius(radius)
{
	
}

DiscCollider2D::DiscCollider2D( const Vec2& worldPos, float radius )
	:m_worldPosition(worldPos)
	,m_radius(radius)
{
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

bool DiscCollider2D::Intersects( const Collider2D* other ) const
{
	//TODO
	switch( other->m_type )
	{
		case COLLIDER2D_DISC:{
			DiscCollider2D* discOther =(DiscCollider2D*)other;
			Vec2 disp = discOther->m_worldPosition - m_worldPosition;
			float distSq = disp.GetLengthSquared();
			if( distSq > (discOther->m_radius + m_radius) * (discOther->m_radius + m_radius) ) {
				return false;
			}
			else {
				return true;
			}
		}

		default:
			return false;
	}
	
}

void DiscCollider2D::SetPosition( Vec2 pos )
{
	m_worldPosition = pos;
}

void DiscCollider2D::DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor )
{
	// TODO test and debug 
	ctx->DrawCircle( m_worldPosition, m_radius, 1, borderColor );
	ctx->DrawFilledCircle( m_worldPosition, ( m_radius - 0.9f ), fillColor );
}