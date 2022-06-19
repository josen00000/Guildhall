#include "ConvexPoly2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/ConvexHull2.hpp"


ConvexPoly2::ConvexPoly2( std::vector<Vec2> points )
{
	m_points = points;
}

ConvexPoly2 ConvexPoly2::MakeConvexPolyFromPointCloud( std::vector<Vec2> points )
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
				if( distSqarB >= 0 ) {
					if( distSqarB > distSqarA ) {
						coLinear.push_back( next );
						next = tempPoint;
					}
					else {
						coLinear.push_back( tempPoint );
					}
				}
			}

		}

		bool isFinished = false;
		for( int i = 0; i < coLinear.size(); i++ ) {
			if( coLinear[i] == start ) {
				isFinished = true;
				break;
			}
		}
		if( next == start || isFinished ) {
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
	ConvexPoly2 tempPoly =  ConvexPoly2( result ); // doesn't work after delete empty constructor
	return tempPoly;
}

ConvexPoly2 ConvexPoly2::MakeConvexPolyFromConvexHull( ConvexHull2 hull )
{
	std::vector<Vec2> intersectPoints = hull.GetConvexPolyPoints();
	ConvexPoly2 poly = ConvexPoly2::MakeConvexPolyFromPointCloud( intersectPoints );
	return poly;
}

ConvexPoly2 ConvexPoly2::MakeConvexPolyFromAABB2( AABB2 const& box )
{
	std::vector<Vec2> points;
	points.push_back( box.mins );
	points.push_back( Vec2( box.maxs.x, box.mins.y ) );
	points.push_back( box.maxs );
	points.push_back( Vec2( box.mins.x, box.maxs.y ) );
	return ConvexPoly2( points );
}

bool ConvexPoly2::IsPointInside( Vec2 point ) const
{
	return true;
}

bool ConvexPoly2::IsPossibleIntersectWithLine( LineSegment2 line ) const
{
	Vec2 polygonCenter = GetCenter();
	float polygonDiscRadius = GetBounderDiscRadius();
	return line.IsLineIntersectWithDisc( polygonCenter, polygonDiscRadius );
}

Vec2 ConvexPoly2::GetCenter() const
{
	Vec2 result = Vec2::ZERO;
	for( int i = 0; i < m_points.size(); i++ ) {
		result += m_points[i];
	}
	if( m_points.size() > 0 ) {
		result /= (float)m_points.size();
	}
	return result;
}

float ConvexPoly2::GetLongestDistance() const
{
	float longestDistSqr = 0.f;
	Vec2 center = GetCenter();
	for( int i = 0; i < m_points.size(); i++ ) {
		float distSqr = GetDistanceSquared2D( center, m_points[i] );
		if( distSqr > longestDistSqr ) {
			longestDistSqr = distSqr;
		}
	}

	return (float)sqrt( longestDistSqr );
}

float ConvexPoly2::GetShortestDistanceToEdge( Vec2 const& point ) const
{
	Vec2 pointOnEdge = GetClosestPointOnEdges( point ); 
	return ( pointOnEdge - point).GetLength();
}

float ConvexPoly2::GetBounderDiscRadius() const
{
	Vec2 center = GetCenter();
	float LongestDistSq = 0;
	for( Vec2 point : m_points ) {
		float tempDistSq = GetDistanceSquared2D( point, center );
		if( tempDistSq > LongestDistSq ) {
			LongestDistSq = tempDistSq;
		}
	}
	return (float)sqrt( LongestDistSq );
}

float ConvexPoly2::GetArea() const
{
	float a = 0;
	float b = 0;
	for( int i = 0; i < m_points.size(); i++ ) {
		Vec2 startPos = m_points[i];
		Vec2 endPos = ( i == m_points.size() - 1 ? m_points[0] : m_points[i + 1] );
		a += startPos.x * endPos.y;
		b += startPos.y * endPos.x;
	}
	return 0.5f * (a - b);
}

void  ConvexPoly2::GetPoints( std::vector<Vec2>& points ) const
{
	points.resize( m_points.size() );
	for( int i = 0; i < m_points.size(); i++ ) {
		points[i] = m_points[i];
	}
}

void ConvexPoly2::GetEdges( std::vector<LineSegment2>& edges ) const
{
	edges.resize( m_points.size() );
	for( int i = 0; i < m_points.size() - 1; i++ ) {
		edges.emplace_back( m_points[i], m_points[i + 1] );
	}
	edges.emplace_back( m_points.back(), m_points[0] );
}

Vec2 ConvexPoly2::GetClosestPointOnEdges( Vec2 const& point ) const
{
	LineSegment2 closestEdge;
	float shortestDist = -1.f;
	for( int i = 0; i < m_points.size(); i++ ) {
		int nextPointIndex = i + 1;
		LineSegment2 edge;
		if( nextPointIndex == m_points.size() ) {
			nextPointIndex = 0;
		}

		edge = LineSegment2( m_points[i], m_points[nextPointIndex] );
		float pointToEdgeDist = edge.GetLengthOfPointToLineSegment( point ); 
		if( pointToEdgeDist < shortestDist || shortestDist > 0.f ) {
			closestEdge = edge;
			shortestDist = pointToEdgeDist;
		}
	}
	return closestEdge.GetNearestPoint( point );
}

void ConvexPoly2::SetPoints( std::vector<Vec2>& points )
{
	m_points = points;
}

void ConvexPoly2::SetCenterPos( Vec2 const& center )
{
	Vec2 currentCenter = GetCenter();
	Vec2 diff = center - currentCenter;
	for( int i = 0; i < m_points.size(); i++ ) {
		m_points[i] += diff;
	}
}

