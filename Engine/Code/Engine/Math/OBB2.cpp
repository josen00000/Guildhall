#include "OBB2.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"




OBB2::OBB2( const AABB2& asAxisAlignedBox, float orientationDegrees )
{
	m_center = asAxisAlignedBox.GetCenter();
	m_halfDimensions = asAxisAlignedBox.GetDimensions() / 2;
	m_iBasisNormal = GetNormalDirectionWithDegrees( orientationDegrees );
}


OBB2::OBB2( const Vec2& fullDimensions, const Vec2& center, float orientationDegrees )
	:m_center(center)
{
	m_halfDimensions = fullDimensions / 2;
	m_iBasisNormal = GetNormalDirectionWithDegrees( orientationDegrees );
}

OBB2::OBB2( const Vec2& fullDimensions, const Vec2& center, const Vec2& iBasisNormal /*= Vec2(1.f, 0.f) */ )
	:m_center(center)
	,m_iBasisNormal(iBasisNormal)
{
	m_halfDimensions = fullDimensions / 2;
}

bool OBB2::IsPointInside( const Vec2& point ) const
{
	// 1 transfer point into obb coordinate
	// normal aabb is point inside
	Vec2 pointInOBBCoords = TransformPointIntoOBBCoords( point );
	if( pointInOBBCoords.x < m_halfDimensions.x && pointInOBBCoords.x > -m_halfDimensions.x && pointInOBBCoords.y < m_halfDimensions.y && pointInOBBCoords.y > -m_halfDimensions.y){
		return true;
	}
	else{
		return false;
	}
}

const Vec2 OBB2::GetCenter() const
{
	return m_center;
}

const Vec2 OBB2::GetDimensions() const
{
	return m_halfDimensions * 2;
}

const Vec2 OBB2::GetIBasisNormal() const
{
	return m_iBasisNormal;
}

const Vec2 OBB2::GetJBasisNormalized() const
{
	Vec2 jBasis;
	jBasis.x = -m_iBasisNormal.y;
	jBasis.y = m_iBasisNormal.x;
	return jBasis;
}

const Vec2 OBB2::GetLeftDownCornerForIBasis() const
{
	Vec2 jBasis = GetJBasisNormalized();
	Vec2 result = m_center - ( m_iBasisNormal * m_halfDimensions.x ) - ( jBasis * m_halfDimensions.y );
	return result;
}

const Vec2 OBB2::GetRightDownCornerForIBasis() const
{
	Vec2 jBasis = GetJBasisNormalized();
	Vec2 result = m_center + ( m_iBasisNormal * m_halfDimensions.x ) - ( jBasis * m_halfDimensions.y );
	return result;
}

const Vec2 OBB2::GetLeftUpCornerForIBasis() const
{
	Vec2 jBasis = GetJBasisNormalized();
	Vec2 result = m_center - ( m_iBasisNormal * m_halfDimensions.x ) + ( jBasis * m_halfDimensions.y );
	return result;
}

const Vec2 OBB2::GetrightUpCornerForIBasis() const
{
	Vec2 jBasis = GetJBasisNormalized();
	Vec2 result = m_center + ( m_iBasisNormal * m_halfDimensions.x ) + ( jBasis * m_halfDimensions.y );
	return result;
}

float OBB2::GetOrientationDegrees() const
{
	return m_iBasisNormal.GetAngleDegrees();
}


float OBB2::GetOuterRadius() const
{
	float result = m_halfDimensions.x + m_halfDimensions.y;
	return result;
}

float OBB2::GetInnerRadius() const
{
	float result = m_halfDimensions.x;
	if( result > m_halfDimensions.y ){
		result = m_halfDimensions.y;
	}
	return result;
}

void OBB2::GetCornerPositions( Vec2* out_fourPoints ) const
{
	out_fourPoints[0] = GetLeftDownCornerForIBasis();
	out_fourPoints[1] = GetLeftUpCornerForIBasis();
	out_fourPoints[2] = GetrightUpCornerForIBasis();
	out_fourPoints[3] = GetRightDownCornerForIBasis();
}

const Vec2 OBB2::GetNearestPoint( const Vec2& referencePos ) const
{
	Vec2 pointInOBBCoords = TransformPointIntoOBBCoords( referencePos );
	float nearestPointX = ClampFloat( -m_halfDimensions.x, m_halfDimensions.x, pointInOBBCoords.x );
	float nearestPointY = ClampFloat( -m_halfDimensions.y, m_halfDimensions.y, pointInOBBCoords.y );
	Vec2 nearestPointInOBB = Vec2( nearestPointX, nearestPointY );
	Vec2 nearestPointInWorld = TransformPointIntoWorldCoords( nearestPointInOBB );
	return nearestPointInWorld;
}

const Vec2 OBB2::GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const
{
	float pointXInOBB = RangeMapFloat( 0, 1, -m_halfDimensions.x, m_halfDimensions.x, uvCoordsZeroToOne.x );
	float pointYInOBB = RangeMapFloat( 0, 1, -m_halfDimensions.y, m_halfDimensions.y, uvCoordsZeroToOne.y );
	Vec2 pointInOBB = Vec2( pointXInOBB, pointYInOBB );
	Vec2 pointInWorld = TransformPointIntoWorldCoords( pointInOBB );
	return pointInWorld;
}

const Vec2 OBB2::GetUVForPoint( const Vec2& point ) const
{
	Vec2 pointInOBB = TransformPointIntoOBBCoords( point );
	float uvPointX = RangeMapFloat( -m_halfDimensions.x, m_halfDimensions.x, 0, 1, pointInOBB.x );
	float uvPointY = RangeMapFloat( -m_halfDimensions.y, m_halfDimensions.y, 0, 1, pointInOBB.y );
	return Vec2( uvPointX, uvPointY );
}

void OBB2::RotateDegrees( float deltaDegrees )
{
	float cosD = CosDegrees( deltaDegrees );
	float sinD = SinDegrees( deltaDegrees );
	float temX = cosD * m_iBasisNormal.x - sinD * m_iBasisNormal.y;
	float temY = sinD * m_iBasisNormal.x + cosD * m_iBasisNormal.y;
	m_iBasisNormal = Vec2( temX, temY );
}

void OBB2::SetOrientationDegrees( float orientationDegrees )
{
	m_iBasisNormal.SetPolarDegrees(orientationDegrees,1);
}

void OBB2::Translate( const Vec2& translation )
{
	float temX = m_center.x + translation.x;
	float temY = m_center.y + translation.y;
	m_center = Vec2( temX, temY );
}

void OBB2::SetCenter( const Vec2& newCenter )
{
	m_center = newCenter;
}

void OBB2::SetDimensions( const Vec2& newDimensions )
{
	m_halfDimensions = newDimensions / 2;
}

void OBB2::StretchToIncludePoint( const Vec2& point )
{
	if( IsPointInside( point )){ return; }
	
	Vec2 jBasis = GetJBasisNormalized();

	Vec2 pointInOBBCoords = TransformPointIntoOBBCoords( point );
	Vec2 minPoint = Vec2( -m_halfDimensions.x, -m_halfDimensions.y );
	Vec2 maxPoint = Vec2( m_halfDimensions.x, m_halfDimensions.y );
	
	if( pointInOBBCoords.x < minPoint.x){
		minPoint.x = pointInOBBCoords.x;
	}
	if( pointInOBBCoords.x > maxPoint.x ) {
		maxPoint.x = pointInOBBCoords.x;
	}
	if( pointInOBBCoords.y < minPoint.y ) {
		minPoint.y = pointInOBBCoords.y;
	}
	if( pointInOBBCoords.y > maxPoint.y ) {
		maxPoint.y = pointInOBBCoords.y;
	}

	m_halfDimensions = (maxPoint - minPoint) / 2;
	Vec2 tempCenter = (maxPoint + minPoint) / 2;
	m_center = m_center + tempCenter.x * m_iBasisNormal + tempCenter.y * jBasis;

}

void OBB2::Fix()
{
	if( m_halfDimensions.x < 0 ){
		m_halfDimensions.x = abs( m_halfDimensions.x );
	}
	if( m_halfDimensions.y < 0 ) {
		m_halfDimensions.y = abs( m_halfDimensions.y );
	}
	
	m_iBasisNormal == Vec2::ZERO;
	m_iBasisNormal = Vec2( 1, 0 );
}

Vec2 OBB2::TransformPointIntoOBBCoords( const Vec2& point ) const
{
	Vec2 jBasis = GetJBasisNormalized();
	Vec2 tempVec = point - m_center;
	float resultX = GetProjectedLength2D( tempVec, m_iBasisNormal); 
	Vec2 tempX = resultX * m_iBasisNormal;
	Vec2 tempY = tempVec - tempX;
	float resultY = tempY.GetLength();
	float dotProduct = DotProduct2D(tempY, jBasis);
	if(dotProduct < 0){
		resultY = -resultY;
	}
	return Vec2( resultX, resultY );
}

Vec2 OBB2::TransformPointIntoWorldCoords( const Vec2& pointInOBB ) const
{
	Vec2 jBasis = GetJBasisNormalized();
	float tempXInWorld = m_iBasisNormal.x * pointInOBB.x + jBasis.x * pointInOBB.y + m_center.x;
	float tempYInWorld = m_iBasisNormal.y * pointInOBB.x + jBasis.y * pointInOBB.y + m_center.y;
	return Vec2( tempXInWorld, tempYInWorld );
}

void OBB2::AddVertsForOBB2D( std::vector<Vertex_PCU>& vertices, const OBB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
{
	Vertex_PCU leftDown = Vertex_PCU(bound.GetLeftDownCornerForIBasis(), tintColor, uvAtMins );
	Vertex_PCU leftUp = Vertex_PCU( bound.GetLeftUpCornerForIBasis(), tintColor, Vec2(uvAtMins.x, uvAtMaxs.y) );
	Vertex_PCU rightDown = Vertex_PCU( bound.GetRightDownCornerForIBasis(), tintColor, Vec2(uvAtMaxs.x, uvAtMins.y) );
	Vertex_PCU rightUp = Vertex_PCU( bound.GetrightUpCornerForIBasis(), tintColor, uvAtMaxs );

	//triangle 1
	vertices.push_back( leftDown );
	vertices.push_back( leftUp );
	vertices.push_back( rightUp );
	//triangle 2
	vertices.push_back( leftDown );
	vertices.push_back( rightDown );
	vertices.push_back( rightUp );
}

// void OBB2::operator=( const OBB2& assignFrom )
// {
// 	
// }

bool OBB2::operator==( const OBB2& compareWith ) const
{
	if( m_center != compareWith.m_center ){
		return false;
	}
	if( m_halfDimensions != compareWith.m_halfDimensions){
		return false;
	}
	if( m_iBasisNormal != compareWith.m_iBasisNormal){
		return false;
	}

	return true;
}

