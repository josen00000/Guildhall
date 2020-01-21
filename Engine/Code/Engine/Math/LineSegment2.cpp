#include "LineSegment2.hpp"
#include "Engine/Math/MathUtils.hpp"

LineSegment2::LineSegment2( const Vec2& start, const Vec2& end )
	:m_start(start)
	,m_end(end)
{
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

void LineSegment2::SetStartPos( const Vec2& start )
{
	m_start = start;
}

void LineSegment2::SetEndPos( const Vec2& end )
{
	m_end = end;

}
