#include "PolygonCollider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Math/Disc2.hpp"


PolygonCollider2D::PolygonCollider2D( Polygon2 polygon )
	:m_polygon(polygon)
{
	m_type = COLLIDER2D_POLYGON;
	m_worldPos = m_polygon.GetMassCenter();
}

PolygonCollider2D::PolygonCollider2D( std::vector <Vec2> points )
{

	m_polygon = Polygon2( points );
	m_worldPos = m_polygon.GetMassCenter();
	m_type = COLLIDER2D_POLYGON;
}

PolygonCollider2D::PolygonCollider2D()
{
	m_type = COLLIDER2D_POLYGON;
}

PolygonCollider2D::~PolygonCollider2D()
{

}

void PolygonCollider2D::UpdateWorldShape()
{

}

Vec2 PolygonCollider2D::GetClosestPoint( const Vec2& pos ) const
{
	Vec2 point = m_polygon.GetClosestPoint( pos );
	return point;
}

bool PolygonCollider2D::Contains( const Vec2& pos ) const
{
	if( m_polygon.Contains( pos ) ) {
		return true;
	}
	else {
		return false;
	}
}

Disc2 PolygonCollider2D::GetWorldBounds() const
{
	Disc2 result;
	result.m_center = m_worldPos;
	result.m_radius = m_polygon.GetLongestDistance();
	return result;
}

// bool PolygonCollider2D::Intersects( const Collider2D* other ) const
// {
// 	switch( other->m_type )
// 	{
// 	case COLLIDER2D_DISC: {
// 		DiscCollider2D* discCol = (DiscCollider2D*)other;
// 		Vec2 closestPoint = GetClosestPoint( discCol->m_worldPosition );
// 		float distSqr = GetDistanceSquared2D( closestPoint, discCol->m_worldPosition );
// 		if( distSqr > (discCol->m_radius * discCol->m_radius) ) {
// 			return false;
// 		}
// 		else {
// 			return true;
// 		}
// 	}
// 	case COLLIDER2D_POLYGON: {
// 		return false;
// 	}
// 
// 	default:
// 		return false;
// 	}
// 	
// 
//}

void PolygonCollider2D::SetPosition( Vec2 pos )
{
	m_polygon.SetCenter( pos );
	m_worldPos = pos;
}

void PolygonCollider2D::DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor )
{
	Vec2 startPos = m_polygon.GetEdge( 0 ).GetStartPos() + m_worldPos;
	std::vector<Vertex_PCU> tempVertices;
	
	for( int edgeIndex = 0; edgeIndex < m_polygon.GetEdgeCount(); edgeIndex++ ) {
		LineSegment2 lineSeg = m_polygon.GetEdge( edgeIndex );
		lineSeg.SetStartPos( lineSeg.GetStartPos() + m_worldPos );
		lineSeg.SetEndPos( lineSeg.GetEndPos() + m_worldPos );
		ctx->DrawLine( lineSeg, 1.f, borderColor );

		if( edgeIndex == 0 ){ continue; }
		else {
			tempVertices.push_back( Vertex_PCU( Vec3( startPos ), fillColor, Vec2::ZERO ) ); 
			tempVertices.push_back( Vertex_PCU( Vec3( lineSeg.GetStartPos() ), fillColor, Vec2::ZERO ) );
			tempVertices.push_back( Vertex_PCU( Vec3( lineSeg.GetEndPos() ), fillColor, Vec2::ZERO ) );
		}
	}
	ctx->DrawVertexVector( tempVertices );
	Disc2 worldBound = GetWorldBounds();
	ctx->DrawCircle( worldBound.m_center, worldBound.m_radius, 0.5f, Rgba8::RED );
}

