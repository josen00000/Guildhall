#include "Capsule2.hpp"
#include "Engine/Math/MathUtils.hpp"

Capsule2::Capsule2( const Vec2& start, const Vec2& end, float radius )
	:m_start(start)
	,m_end(end)
	,m_radius(radius)
{

}

Capsule2::Capsule2( const Vec2& start, const Vec2& diretion, float length, float radius )
	:m_start(start)
	,m_radius(radius)
{
	m_end = start + diretion * length;
}

Vec2 Capsule2::GetStartPos() const
{
	return m_start;
}

Vec2 Capsule2::GetEndPos() const
{
	return m_end;
}

float Capsule2::GetRadius() const
{
	return m_radius;
}

float Capsule2::GetLength() const
{
	Vec2 capsuleDisp = m_end - m_start;
	float length = capsuleDisp.GetLength();
	return length;
}

Vec2 Capsule2::GetNormalizedDirection() const
{
	Vec2 direction = m_end - m_start;
	direction.Normalize();
	return direction;
}

float Capsule2::GetlengthSquared() const
{
	Vec2 capsuleDisp = m_end - m_start;
	float lengthSquared = capsuleDisp.GetSquaredLength();
	return lengthSquared;
}

Vec2 Capsule2::GetNearestPoint( const Vec2& refPos ) const
{
	if( IsPointInSide( refPos )){
		return refPos;
	}
	
	Vec2 disp_startToRef = refPos - m_start;
	Vec2 capsuleDirection = GetNormalizedDirection();
	Vec2 result;
	float dotProduct1 = DotProduct2D( disp_startToRef, capsuleDirection );
	if( dotProduct1 < 0 ){
		Vec2 tempDirection1 = disp_startToRef.GetNormalized();
		result = m_start + tempDirection1 * m_radius;
	}
	else{
		Vec2 disp_endToRef = refPos - m_end;
		float dotProduct2 = DotProduct2D( disp_endToRef, capsuleDirection );
		if( dotProduct2 > 0 ){
			Vec2 tempDirection2 = disp_endToRef.GetNormalized();
			result = m_end + tempDirection2 * m_radius;
		}
		else{
			Vec2 projectedToCapsuleDirection = GetProjectedOnto2D( disp_startToRef, capsuleDirection );
			Vec2 tempVec2 = disp_startToRef - projectedToCapsuleDirection;
			tempVec2.ClampLength( m_radius );
			result = m_start + projectedToCapsuleDirection + tempVec2;
		}
	}
	
	return result;
	
}

bool Capsule2::IsPointInSide( const Vec2& refPos ) const
{
	OBB2 capsuleOBB = GetCapsuleOBB2();
	if( capsuleOBB.IsPointInside( refPos )){
		return true;
	}
	else{
		float radiusSquare = m_radius * m_radius;
		Vec2 tempDisp = refPos - m_start;
		if( tempDisp.GetSquaredLength() < radiusSquare ){
			return true;
		}
		else{
			Vec2 tempDisp2 = refPos - m_end;
			if( tempDisp2.GetSquaredLength() < radiusSquare){
				return true;
			}
			else{
				return false;
			}
		}
	}
}

const OBB2 Capsule2::GetCapsuleOBB2() const
{
	Vec2 direction = GetNormalizedDirection();
	Vec2 center = ( m_end + m_start ) / 2;
	float width = (m_end - m_start).GetLength();
	float height = m_radius * 2;
	Vec2 dimension = Vec2( width, height );
	OBB2 result = OBB2( dimension, center, direction );
	return result;

}

void Capsule2::SetStartPos( const Vec2& start )
{
	m_start = start;
}

void Capsule2::SetEndPos( const Vec2& end )
{
	m_end = end;
}

void Capsule2::SetRadius( float radius )
{
	m_radius = radius;
}

void Capsule2::SetLength( float length )
{
	Vec2 capsuleDirction = GetNormalizedDirection();
	m_end = m_start + capsuleDirction * length;
}
