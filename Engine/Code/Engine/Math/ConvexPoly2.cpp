#include "ConvexPoly2.hpp"
#include "Engine/Math/MathUtils.hpp"

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

bool ConvexPoly2::IsPointInside( Vec2 point ) const
{
	return true;
}

Vec2 ConvexPoly2::GetCenter() const
{
	Vec2 result = Vec2::ZERO;
	for( int i = 0; i < m_points.size(); i++ ) {
		result += m_points[i];
	}
	if( m_points.size() > 0 ) {
		result /= m_points.size();
	}
	return result;
}
