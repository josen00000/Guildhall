#include "LineSegment2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/FloatRange.hpp"


LineSegment2::LineSegment2( const Vec2& start, const Vec2& end )
	:m_start(start)
	,m_end(end)
{
}

LineSegment2::LineSegment2( Vec2 const& start, Vec2 const& fwdDirt, float dist )
	:m_start(start)
{
	m_end = m_start + fwdDirt * dist;
}

LineSegment2 LineSegment2::ClipSegmentToSegment( LineSegment2 toClip, LineSegment2 refSeg )
{
	Vec2 onto = refSeg.GetEndPos() - refSeg.GetStartPos();
	Vec2 source1 = toClip.GetStartPos() - refSeg.GetStartPos();
	Vec2 source2 = toClip.GetEndPos() - refSeg.GetStartPos();

	float refLength		= refSeg.GetLength();
	float startLength	=  GetProjectedLength2D( source1, onto );
	float endLength		=  GetProjectedLength2D( source2, onto );

	if( startLength > endLength ){
		Vec2 temp = toClip.GetStartPos();
		toClip.SetStartPos( toClip.GetEndPos() );
		toClip.SetEndPos( temp );
		
		float tempf = startLength;
		startLength = endLength;
		endLength = tempf;
	}
	
	FloatRange toClipRange = FloatRange( startLength, endLength );
	FloatRange refRange		= FloatRange( 0.f, refLength );
	FloatRange clippedRange = FloatRange::GetIntersectRange( toClipRange, refRange );

	if( !clippedRange.IsInRange( refRange ) ) {
		return LineSegment2( Vec2::ZERO, Vec2::ZERO );
	}

	if( clippedRange.minimum == clippedRange.maximum ) {
		return toClip;
	}

	Vec2 clippedStart	= RangeMapFromFloatToVec2( toClipRange.minimum, toClipRange.maximum, toClip.m_start, toClip.m_end, clippedRange.minimum );
	Vec2 clippedEnd		= RangeMapFromFloatToVec2( toClipRange.minimum, toClipRange.maximum, toClip.m_start, toClip.m_end, clippedRange.maximum );

	return LineSegment2( clippedStart, clippedEnd );
}

LineSegment2 LineSegment2::ClipSegmentToSegmentAlongDirection( LineSegment2 toClip, Vec2 start, Vec2 direction )
{
	Vec2 onto = start + direction;
	Vec2 source1 = toClip.GetStartPos() - start;
	Vec2 source2 = toClip.GetEndPos() - start;

	float startLength	= GetProjectedLength2D( source1, onto );
	float endLength		= GetProjectedLength2D( source2, onto );

	FloatRange toClipRange = FloatRange( startLength, endLength );
	FloatRange refRange = FloatRange( 0.f, (float)INT_MAX );

	FloatRange clippedRange = FloatRange::GetIntersectRange( toClipRange, refRange );

	Vec2 clippedStart	= RangeMapFromFloatToVec2( toClipRange.minimum, toClipRange.maximum, toClip.m_start, toClip.m_end, clippedRange.minimum );
	Vec2 clippedEnd		= RangeMapFromFloatToVec2( toClipRange.minimum, toClipRange.maximum, toClip.m_start, toClip.m_end, clippedRange.maximum );

	return LineSegment2( clippedStart, clippedEnd );

}

bool LineSegment2::IsPointMostlyInEdge( Vec2 point ) const
{
	Vec2 pointInEdge = GetNearestPoint( point );
	float dist = ( point - pointInEdge ).GetLength() ;
	return IsFloatMostlyEqual( dist, 0.f );
}

bool LineSegment2::IsPointMostlyInStraightLine( Vec2 point ) const
{
	if( point.x == m_start.x || point.x == m_end.x ) {
		if( point.x == m_start.x && point.x == m_end.x ) {
			return true;
		}
		else {
			return false;
		}
	}
// 	if( point.y == m_start.y || point.y == m_end.y ) {
// 		if( point.y == m_start.y && point.y == m_end.y ) {
// 			return true;
// 		}
// 		else {
// 			return false;
// 		}
// 	}

	return IsFloatMostlyEqual( ( point.y - m_start.y ) / ( point.x - m_start.x ), ( point.y - m_end.y ) / ( point.x - m_end.x ) );
}

bool LineSegment2::IsLineOverlapWith( LineSegment2 lineB ) const
{
	// A is self linesegment, B is other line
	if( !IsLineParallelWith( lineB ) ) { return false; }

	float lengthA = GetLength();
	Vec2 disp_BS_AS = lineB.GetStartPos() - GetStartPos();
	Vec2 dispB = lineB.GetDisplacement();
	Vec2 dirtA = GetNormalizedDirection();
	float lengthBInA = DotProduct2D( dispB, dirtA );
	float lengthBSInA = DotProduct2D( disp_BS_AS, dirtA );
	FloatRange range1( 0.f, lengthA );
	FloatRange range2( lengthBSInA, lengthBSInA + lengthBInA );
	return range1.DoesOverlap( range2 );
}

bool LineSegment2::IsLineParallelWith( LineSegment2 line ) const
{
	Vec2 dirt1 = line.GetDisplacement();
	Vec2 dirt2 = GetDisplacement();
	float length1 = dirt1.GetLength();
	float length2 = dirt2.GetLength();
	if( length1 == 0 || length2 == 0 ){ return false; }

	dirt1 /= length1;
	dirt2 /= length2;

	return ( dirt1 == dirt2 );
}

bool LineSegment2::IsLineIntersectWithDisc( Vec2 center, float radius )
{
	Vec2 nearestPoint = GetNearestPoint( center );
	float distSq = GetDistanceSquared2D( nearestPoint, center );
	return distSq < ( radius * radius );
}

Vec2 LineSegment2::GetStartPos() const
{
	return m_start;
}

Vec2 LineSegment2::GetEndPos() const
{
	return m_end;
}

Vec2 LineSegment2::GetNormalizedDirection() const
{
	Vec2 direction = m_end - m_start;
	direction.Normalize();
	return direction;
}

Vec2 LineSegment2::GetDisplacement() const
{
	return ( m_end - m_start );
}

float LineSegment2::GetLengthOfPointToLineSegment( const Vec2& refPos ) const
{
	Vec2 nearestPoint = GetNearestPoint( refPos );
	Vec2 tempDisp = refPos - nearestPoint;
	float length = tempDisp.GetLength();
	return length;
}

float LineSegment2::GetLength() const
{
	Vec2 direction = m_end - m_start;
	float length = direction.GetLength();
	return length;
}

float LineSegment2::GetSlope() const
{
	return ( m_end.y - m_start.y ) / ( m_end.x - m_start.x );
}

Vec2 LineSegment2::GetNearestPoint( const Vec2& refPos ) const
{
	Vec2 result;
	Vec2 tempDisp1 = refPos - m_start;
	Vec2 direction = m_end	- m_start;
	float tempDotProduct1 = DotProduct2D( tempDisp1, direction );
	if( tempDotProduct1 < 0 ){
		return m_start;
	}
	else{
		Vec2 tempDisP2 = refPos - m_end;
		float tempDotProduct2 = DotProduct2D( tempDisP2, direction );
		if( tempDotProduct2 > 0 ){
			result = m_end;
		}
		else{
			Vec2 projectedVec2 = GetProjectedOnto2D( tempDisp1, direction );
			result = m_start + projectedVec2;
		}
	}
	return result;
}

OBB2 LineSegment2::GetLineOBB2WithThickness( float thickness ) const
{
	Vec2 dimension = Vec2( GetLength(), thickness );
	Vec2 center = ( m_end - m_start ) / 2.f;
	Vec2 direction = GetNormalizedDirection();
	OBB2 result = OBB2( dimension, center, direction );
	return result;
}

void LineSegment2::SetStartPos( const Vec2& start )
{
	m_start = start;
}

void LineSegment2::SetEndPos( const Vec2& end )
{
	m_end = end;

}
