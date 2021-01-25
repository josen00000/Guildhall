#include "Polygon2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"

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
	bool isFinished = false;
	while( true ) {
		int nextIndex = 0;
		Vec2 next = points[nextIndex];
		while( next == current )
		{
			nextIndex++;
			next = points[nextIndex];
		}

		std::vector<Vec2> coLinear;
		for( int pointIndex = 0; pointIndex < points.size(); pointIndex++ ) {
			const Vec2& tempPoint = points[pointIndex];
			if( tempPoint == current || tempPoint == next ) { continue; }

			float crossPro = CrossProduct2D( current, next, tempPoint );
			if( crossPro < 0 ) {
				next = tempPoint;
				coLinear.clear();
			}
			else if( crossPro == 0 ) {
				float distSqarA = GetDistanceSquared2D( current, next );
				float distSqarB = GetSignedDistanceSquared2D( current, tempPoint, next );
				if( distSqarB >= 0 ){
					if( distSqarB > distSqarA ) {
						coLinear.push_back( next );
						next = tempPoint;
					}
					else {
						coLinear.push_back( tempPoint );
					}
				}
			}
			if( next == start ) { 
				isFinished = true;
				coLinear.clear();
				break; 
			}
		}

		if( isFinished ) {
			break;
		}

		for( int coIndex = 0; coIndex < coLinear.size(); coIndex++ ) {
			result.push_back( coLinear[coIndex] );
		}

		result.push_back( next );
		current = next;
	}

	for( int i = 0; i < result.size(); i++ ) {
		for( int j = i; j < result.size(); j++ ) {
			if( IsVec2MostlyEqual( result[i], result[j] ) && i != j ) {
				result.erase( result.begin() + j );
			}
		}
	}
	Polygon2 tempPoly = Polygon2( result ); // doesn't work after delete empty constructor
	return tempPoly;
}

Polygon2 Polygon2::MakeConvexFromAABB2( AABB2 box )
{
	std::vector<Vec2> points;
	points.push_back( box.mins );
	points.push_back( Vec2( box.maxs.x, box.mins.y ) );
	points.push_back( box.maxs );
	points.push_back( Vec2( box.mins.x, box.maxs.y ) );
	return Polygon2( points );
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

bool Polygon2::IsPointInside( Vec2 point ) const
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

float Polygon2::GetDistanceToCenter( Vec2 point ) const
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



float Polygon2::GetDistanceToEdge( Vec2 point ) const
{
	Vec2 pointOnEdge = GetClosestPointOnEdges( point );
	return ( pointOnEdge - point).GetLength(); 
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
	if( IsPointInside( point ) ){ return point; }

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
	float shortestDist = -1.f;
	Vec2 closestPoint;
	for( int edgeIndex = 0; edgeIndex < m_edges.size(); edgeIndex++ ) {
		LineSegment2 edge = m_edges[edgeIndex];
		float length = edge.GetLengthOfPointToLineSegment( point );
		if( shortestDist < 0.f || length < shortestDist ) {
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

Vec2 Polygon2::GetEdgeNormal( int edgeIndex ) const
{
	LineSegment2 edge = GetEdge( edgeIndex );
	Vec2 direction = edge.GetNormalizedDirection();
	return direction.GetRotatedMinus90Degrees();
}

std::pair<Vec2, Vec2> Polygon2::GetIntersectPointWithStraightLine( LineSegment2 line )
{
	float lDist = GetLongestDistance();
	if( line.GetLength() < 2 * lDist ) {
		Vec2 lineDirt = line.GetDirection();
		lineDirt.Normalize();
		Vec2 lineCenter = ( line.GetStartPos() + line.GetEndPos() ) / 2;
		line.SetStartPos( lineCenter - lDist * lineDirt );
		line.SetEndPos( lineCenter + lDist * lineDirt );
	}
	Vec2 intersectPoint;
	std::pair<Vec2, Vec2> result;
	int intersectPointNum = 0;
	for( int i = 0; i < m_edges.size(); i++ ) {
		 if( GetIntersectionPointOfTwoLineSegments( intersectPoint, m_edges[i], line ) ) {
			 if( intersectPointNum == 0 ) {
				 result.first = intersectPoint;
				 intersectPointNum++;
			 }
			 else if( intersectPointNum == 1 ){
				result.second = intersectPoint;
				break;
			 }
		 }
	}
	return result;
}

std::vector<Vec2> Polygon2::GetAllPoints()
{
	std::vector<Vec2> result;
	for( int i = 0; i < m_edges.size(); i++ ) {
		result.push_back( m_center + m_edges[i].GetStartPos() );
	}
	return result;
}

LineSegment2 Polygon2::GetEdge( int index ) const
{
	return m_edges[index];
}

LineSegment2 Polygon2::GetEdgeInWorld( int index ) const
{
	LineSegment2 result = GetEdge( index );
	result.SetStartPos( m_center + result.GetStartPos() );
	result.SetEndPos( m_center + result.GetEndPos() );
	return result;
}

bool Polygon2::GetEdgeInWorldWithPoint( Vec2 point, LineSegment2& seg ) const
{
	for( int i = 0; i < m_edges.size(); i++ ) {
		LineSegment2 edge = m_edges[i];
		if( edge.IsPointMostlyInEdge( point ) ){
			seg = edge;
			return true;
		}
	}
	return false;
}

LineSegment2 Polygon2::GetEdgeInWorldWithPoint( Vec2 point ) const
{
	for( int i = 0; i < m_edges.size(); i++ ) {
		LineSegment2 edge = m_edges[i];
		if( edge.IsPointMostlyInEdge( point ) ) {
			return edge;
		}
	}
	return LineSegment2();
}

AABB2 Polygon2::GetQuickOutBox()
{
	std::vector<Vec2> vertices;
	GetAllVerticesInWorld( vertices );
	Vec2 min = vertices[0];
	Vec2 max = vertices[0];
	for( int i = 1; i < vertices.size(); i++ ) {
		Vec2 tempPoint = vertices[i];
		if( tempPoint.x < min.x ) {
			min.x = tempPoint.x;
		}
		if( tempPoint.y < min.y ) {
			min.y = tempPoint.y;
		}

		if( tempPoint.x > max.x ) {
			max.x = tempPoint.x;
		}
		if( tempPoint.y > max.y ) {
			max.y = tempPoint.y;
		}
	}
	return AABB2( min, max );
}

int Polygon2::GetEdgeIndexWithPoint( Vec2 point ) const
{
	for( int i = 0; i < m_edges.size(); i++ ) {
		LineSegment2 edge = m_edges[i];
		if( edge.IsPointMostlyInEdge( point ) ) {
			return i;
		}
	}
	return -1;
}

void Polygon2::GetAllVerticesInWorld( std::vector<Vec2>& vertices ) const
{
	for( int i = 0; i < m_edges.size(); i++ ){
		Vec2 point = m_edges[i].GetStartPos() + m_center;
		vertices.push_back( point );
	}
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


