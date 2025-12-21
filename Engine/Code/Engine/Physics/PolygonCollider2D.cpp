#include "PolygonCollider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/ConvexPoly2.hpp"

PolygonCollider2D::PolygonCollider2D( ConvexPoly2 polygon )
	:m_localPolygon(polygon)
{
	m_type = COLLIDER2D_POLYGON;
	m_worldPos = m_localPolygon.GetCenter();
	m_worldPolygon = m_localPolygon;
}

PolygonCollider2D::PolygonCollider2D( std::vector <Vec2> points )
{

	m_localPolygon = ConvexPoly2( points );
	m_worldPos = m_localPolygon.GetCenter();
	m_type = COLLIDER2D_POLYGON;
	m_worldPolygon = m_localPolygon;
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
	//m_worldPolygon = m_localPolygon;
	std::vector<Vec2> points;
	m_localPolygon.GetPoints( points );
	for( int i = 0; i < points.size(); i++ ) {
		Vec2& point = points[i];
		float radians = point.GetAngleRadians();
		point.SetAngleRadians( radians + m_rigidbody->GetRotationInRadians() );
	}

	m_localPolygon.SetPoints( points );
}

Vec2 PolygonCollider2D::GetClosestPoint( const Vec2& pos ) const
{
	Vec2 point = m_worldPolygon.GetClosestPointOnEdges( pos );
	return point;
}

bool PolygonCollider2D::Contains( const Vec2& pos ) const
{
	if( m_worldPolygon.IsPointInside( pos ) ) {
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
	result.m_radius = m_worldPolygon.GetLongestDistance();
	return result;
}

Vec2 PolygonCollider2D::GetCentroid() const
{
	return m_worldPolygon.GetCenter();
}

void PolygonCollider2D::SetPosition( Vec2 pos )
{
	m_worldPolygon.SetCenterPos( pos );
	m_localPolygon.SetCenterPos( pos );
	m_worldPos = pos;
}

float PolygonCollider2D::CalculateMoment( float mass ) const
{
	float up = 0;

	std::vector<Vec2> points;
	m_worldPolygon.GetPoints( points );

	for( int i = 0; i < points.size(); i++ ) {
		Vec2 start = points[i];
		Vec2 end;
		if( i == points.size() - 1 ) {
			end = points[0];
		}
		else {
			end = points[i+1];
		}

		float crossProduct = abs( Vec2::CrossProduct( end, start ) );
		float b = (DotProduct2D( start, start ) + DotProduct2D( start, end ) + DotProduct2D( end, end ));
		up = up + b * crossProduct;
	}

	float down = 0;

	for( int i = 0; i < points.size(); i++ ) {
		Vec2 start = points[i];
		Vec2 end = ( i == points.size() - 1 ? points[0] : points[i] );
		float crossProduct = abs( Vec2::CrossProduct( end, start ));
		down += crossProduct;
	}
	return ( mass * up ) / ( 6 * down );
}

void PolygonCollider2D::DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor )
{
// 	Vec2 startPos = m_worldPolygon.GetEdge( 0 ).GetStartPos() + m_worldPos;
// 	std::vector<Vertex_PCU> tempVertices;
// 	
// 	for( int edgeIndex = 0; edgeIndex < m_worldPolygon.GetEdgeCount(); edgeIndex++ ) {
// 		LineSegment2 lineSeg = m_worldPolygon.GetEdge( edgeIndex );
// 		lineSeg.SetStartPos( lineSeg.GetStartPos() + m_worldPos );
// 		lineSeg.SetEndPos( lineSeg.GetEndPos() + m_worldPos );
// 		ctx->DrawLine( lineSeg, 1.f, borderColor );
// 
// 		if( edgeIndex == 0 ){ continue; }
// 		else {
// 			tempVertices.push_back( Vertex_PCU( Vec3( startPos ), fillColor, Vec2::ZERO ) ); 
// 			tempVertices.push_back( Vertex_PCU( Vec3( lineSeg.GetStartPos() ), fillColor, Vec2::ZERO ) );
// 			tempVertices.push_back( Vertex_PCU( Vec3( lineSeg.GetEndPos() ), fillColor, Vec2::ZERO ) );
// 		}
// 	}
// 	ctx->DrawVertexVector( tempVertices );
// 	Disc2 worldBound = GetWorldBounds();
	//ctx->DrawCircle( worldBound.m_center, worldBound.m_radius, 0.5f, Rgba8::RED );

	// test draw
	//testRenderLocal( ctx );
}

void PolygonCollider2D::testRenderLocal( RenderContext* ctx )
{
// 	Vec2 startPos = m_localPolygon.GetEdge( 0 ).GetStartPos() + m_worldPos;
// 	std::vector<Vertex_PCU> tempVertices;
// 
// 	for( int edgeIndex = 0; edgeIndex < m_localPolygon.GetEdgeCount(); edgeIndex++ ) {
// 		LineSegment2 lineSeg = m_localPolygon.GetEdge( edgeIndex );
// 		lineSeg.SetStartPos( lineSeg.GetStartPos() + m_worldPos );
// 		lineSeg.SetEndPos( lineSeg.GetEndPos() + m_worldPos );
// 		ctx->DrawLine( lineSeg, 1.f, Rgba8::GREEN );
// 
// 		if( edgeIndex == 0 ) { continue; }
// 		else {
// 			tempVertices.push_back( Vertex_PCU( Vec3( startPos ), Rgba8::YELLOW, Vec2::ZERO ) );
// 			tempVertices.push_back( Vertex_PCU( Vec3( lineSeg.GetStartPos() ), Rgba8::YELLOW, Vec2::ZERO ) );
// 			tempVertices.push_back( Vertex_PCU( Vec3( lineSeg.GetEndPos() ), Rgba8::YELLOW, Vec2::ZERO ) );
// 		}
// 	}
// 	ctx->DrawVertexVector( tempVertices );
// 	Disc2 worldBound = GetWorldBounds();
// 	ctx->DrawCircle( worldBound.m_center, worldBound.m_radius, 0.5f, Rgba8::RED );

}

