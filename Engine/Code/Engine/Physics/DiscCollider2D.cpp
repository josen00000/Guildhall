#include "DiscCollider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

DiscCollider2D::DiscCollider2D( const Vec2& localPos, const Vec2& worldPos, float radius )
	:m_localPosition(localPos)
	,m_worldPosition(worldPos)
	,m_radius(radius)
{
	
}

DiscCollider2D::DiscCollider2D()
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
	return true;
}

void DiscCollider2D::DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor )
{
	//ctx->DrawCircle( m_localPosition, m_radius, 0.1, borderColor );
	//ctx->DrawCircle( m_localPosition,( m_radius - 0.1 ), m_radius )
}
