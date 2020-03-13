#include "Polygon2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

Polygon2::Polygon2( std::vector<Vec2> points )
{
	m_center = GetMassCenter( points );
	for( int pointIndex = 1; pointIndex < points.size(); pointIndex++ ) {
		Vec2 firstPoint = points[pointIndex - 1] - m_center;
		Vec2 secondPoint = points[pointIndex] - m_center;
		m_edges.push_back( LineSegment2( firstPoint, secondPoint ) );
	}
	m_edges.push_back( LineSegment2( points.back() - m_center, points[0] - m_center ) );
}

Polygon2 Polygon2::MakeConvexFromPointCloud( std::vector<Vec2> points )
{
	Vec2 start = points[0];
	for( int pointIndex = 1; pointIndex < points.size(); pointIndex++ ) {
		if( points[pointIndex].x < start.x ) {
			start = points[pointIndex];
		}
	}

	std::vector<Vec2> result;
	result.push_back( start );

	Vec2 current = start;
	while( true ) {
		Vec2 next = points[0];
		std::vector<Vec2> coLinear;
		for( int pointIndex = 1; pointIndex < points.size(); pointIndex++ ) {
			const Vec2& tempPoint = points[pointIndex];
			if( tempPoint == current ) { continue; }

			float crossPro = CrossProduct2D( current, next, tempPoint );
			if( crossPro < 0 ) {
				next = tempPoint;
				coLinear.clear();
			}
			else if( crossPro == 0 ) {
				float distSqarA = GetDistanceSquared2D( current, next );
				float distSqarB = GetDistanceSquared2D( current, tempPoint );
				if( distSqarB > distSqarA ) {
					coLinear.push_back( next );
					next = tempPoint;
				}
				else {
					coLinear.push_back( tempPoint );
				}
			}
		}

		for( int coIndex = 0; coIndex < coLinear.size(); coIndex++ ) {
			result.push_back( coLinear[coIndex] );
		}

		if( next == start ) { break; }

		result.push_back( next );
		current = next;
	}
	Polygon2 tempPoly = Polygon2( result ); // doesn't work after delete empty constructor
	return tempPoly;
}

bool Polygon2::IsValid() const
{
	if( m_edges.size() >= 3 ) {
		return true;
	}
	else {
		return false;
	}
}

bool Polygon2::IsConvex() const
{
	if( !IsValid() ){ return false; }

	for( int edgeIndex = 1; edgeIndex < m_edges.size(); edgeIndex++ ) {
		Vec2 nVecOfEdge = m_edges[edgeIndex-1].GetDirection();
		nVecOfEdge.Rotate90Degrees();
		Vec2 drctnOfEdge = m_edges[edgeIndex].GetDirection();
		if( DotProduct2D( nVecOfEdge, drctnOfEdge ) < 0 ) { return false; }
	}
	
	// Check last edge with first edge
	Vec2 nVecOfEdge = m_edges.back().GetDirection();
	nVecOfEdge.Rotate90Degrees();
	Vec2 drctnOfEdge = m_edges[0].GetDirection();
	if( DotProduct2D( nVecOfEdge, drctnOfEdge ) < 0 ){ return false; }

	return true;
}

bool Polygon2::Contains( Vec2 point ) const
{
	point = point - m_center;
	for( int edgeIndex = 0; edgeIndex < m_edges.size(); edgeIndex++ ) {
		const LineSegment2& edge = m_edges[edgeIndex];
		Vec2 PS = point - edge.GetStartPos();
		Vec2 edgeNormalDrctn = edge.GetDirection().GetRotated90Degrees();
		if( DotProduct2D( PS, edgeNormalDrctn ) < 0 ){ return false; }
	}

	return true;
}

float Polygon2::GetDistance( Vec2 point ) const
{
	Vec2 badCenter = m_center;
	Vec2 disp = point - badCenter;
	return disp.GetLength();
}

float Polygon2::GetLongestDistance() const
{
	float result = 0;
	for( int edgeIndex = 0; edgeIndex < m_edges.size(); edgeIndex++ ) {
		float distSqr = GetDistanceSquared2D( Vec2::ZERO, m_edges[edgeIndex].GetStartPos() );
		if( result < distSqr ) {
			result = distSqr;
		}
	}
	return (float)sqrt( result );
}



Vec2 Polygon2::GetLowestPoint() const
{
	Vec2 point = m_edges[0].GetStartPos();
	for( int edgeIndex = 0; edgeIndex < m_edges.size(); edgeIndex++ ) {
		Vec2 temPoint = m_edges[edgeIndex].GetStartPos();
		if( temPoint.y < point.y ) {
			point = temPoint;
		}
	}
	return point;
}

int Polygon2::GetVertexCount() const
{
	return (int)m_edges.size();
}

int Polygon2::GetEdgeCount() const
{
	return (int)m_edges.size();
}

Vec2 Polygon2::GetClosestPoint( Vec2 point ) const
{
	if( Contains( point ) ){ return point; }

	point = point - m_center;

	LineSegment2 closestEdge;
	float shortestDist = 0;
	Vec2 closestPoint;
	for( int edgeIndex = 0; edgeIndex < m_edges.size(); edgeIndex++ ) {
		LineSegment2 edge = m_edges[edgeIndex];
		float length = edge.GetLengthOfPointToLineSegment( point );
		if( shortestDist == 0 || length < shortestDist ) {
			shortestDist = length;
			closestEdge = edge;
		}
	}
	closestPoint = closestEdge.GetNearestPoint( point );
	return closestPoint + m_center; 
}

Vec2 Polygon2::GetClosestPointOnEdges( Vec2 point ) const
{
	point = point - m_center;

	LineSegment2 closestEdge;
	float shortestDist = 0;
	Vec2 closestPoint;
	for( int edgeIndex = 0; edgeIndex < m_edges.size(); edgeIndex++ ) {
		LineSegment2 edge = m_edges[edgeIndex];
		float length = edge.GetLengthOfPointToLineSegment( point );
		if( shortestDist == 0 || length < shortestDist ) {
			shortestDist = length;
			closestEdge = edge;
		}
	}
	closestPoint = closestEdge.GetNearestPoint( point );
	return closestPoint + m_center;
}

Vec2 Polygon2::GetMassCenter( std::vector<Vec2> rawPoints ) const
{
	//if( m_edges.size() < 3 ){ return Vec2::ZERO; }
	float totalArea = 0;
	Vec2 totalAreaWithCentroid = Vec2::ZERO;
	for( int i = 1 ; i < rawPoints.size() - 1; i++ ){
		// for each triangle
		Vec2 a = rawPoints[0];
		Vec2 b = rawPoints[i];
		Vec2 c = rawPoints[i+1];
		Vec2 triangleCentroid = ( a + b + c ) / 3;
		float triangleArea = GetAreaOfTriangle( a, b, c );
		totalAreaWithCentroid += triangleArea * triangleCentroid;
		totalArea += triangleArea;
	}
	Vec2 result = totalAreaWithCentroid / totalArea;
	return result;
}

LineSegment2 Polygon2::GetEdge( int index ) const
{
	return m_edges[index];
}

void Polygon2::SetEdgesFromPoints( std::vector<Vec2> points )
{
	m_center = GetMassCenter( points );
	m_edges.clear();
	for( int pointIndex = 1; pointIndex < points.size(); pointIndex++ ) {
		Vec2 firstPoint = points[pointIndex - 1];
		Vec2 secondPoint = points[pointIndex];
		m_edges.push_back( LineSegment2( firstPoint, secondPoint ) );
	}
	m_edges.push_back( LineSegment2( points[points.size()-1], points[0] ) );
}

void Polygon2::SetCenter( Vec2 center )
{
	m_center = center;
}


