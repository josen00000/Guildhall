#include "AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"


AABB2::AABB2( const AABB2& copyFrom )
	:mins(copyFrom.mins)
	,maxs(copyFrom.maxs)
{

}
//REVIEW name confusing
AABB2::AABB2( const Vec2& mins, const Vec2& maxs )
	:mins( mins )
	,maxs(maxs )
{

}

AABB2::AABB2( float minX, float minY, float maxX, float maxY )
{
	mins.x=minX;
	mins.y=minY;
	maxs.x=maxX;
	maxs.y=maxY;

}


AABB2::AABB2( const IntVec2& intMins, const IntVec2& intMaxs )
{
	mins.x = (float)intMins.x;
	mins.y = (float)intMins.y;
	maxs.x = (float)intMaxs.x;
	maxs.y = (float)intMaxs.y;
}

AABB2::AABB2( Vec2 const& center, float width, float height )
{
	Vec2 halfDimension = Vec2( ( width / 2 ), ( height / 2 ) );
	mins = center - halfDimension;
	maxs = center + halfDimension;
}

bool AABB2::IsPointInside( const Vec2& point ) const
{
	if(( point.x >= mins.x && point.x <= maxs.x ) && (point.y >= mins.y && point.y <= maxs.y )){
		return true;
	}
	else{
		return false;
	}
}

bool AABB2::IsPointMostInEdge( const Vec2& point ) const
{
	if( IsFloatMostlyEqual( point.x, mins.x ) || IsFloatMostlyEqual( point.x, maxs.x ) ) {
		return( point.y >= mins.y && point.y <= maxs.y );
	}
	else if( IsFloatMostlyEqual( point.y, mins.y ) || IsFloatMostlyEqual( point.y, maxs.y ) ) {
		return( point.x >= mins.x && point.x <= maxs.x );
	}
}

const Vec2 AABB2::GetCenter() const
{
	float centerX = (mins.x+ maxs.x) / 2;
	float centerY = (mins.y + maxs.y) / 2;
	return Vec2( centerX, centerY );
}

const Vec2 AABB2::GetDimensions() const
{
	float dimensionsX = maxs.x - mins.x;
	float dimensionsY = maxs.y - mins.y;
	return Vec2( dimensionsX, dimensionsY );
}

const Vec2 AABB2::GetNearestPoint( const Vec2& referencePosition ) const
{
	Vec2 nearestPoint=Vec2();
	if(referencePosition.x <= mins.x){
		nearestPoint.x = mins.x;
	}
	else if(referencePosition.x < maxs.x){
		nearestPoint.x = referencePosition.x;
	}
	else{
		nearestPoint.x = maxs.x;
	}

	if( referencePosition.y <= mins.y ) {
		nearestPoint.y = mins.y;
	}
	else if( referencePosition.y < maxs.y ) {
		nearestPoint.y = referencePosition.y;
	}
	else {
		nearestPoint.y = maxs.y;
	}
	return nearestPoint;
}

const Vec2 AABB2::GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const
{
	Vec2 AABB2Point = Vec2();
	AABB2Point.x = RangeMapFloat(  0, 1, mins.x, maxs.x, uvCoordsZeroToOne.x );
	AABB2Point.y = RangeMapFloat(  0, 1, mins.y, maxs.y, uvCoordsZeroToOne.y );
	return AABB2Point;
}

const Vec2 AABB2::GetUVForPoint( const Vec2& point ) const
{
	Vec2 uvPoint=Vec2();
	uvPoint.x=RangeMapFloat( mins.x, maxs.x, 0, 1, point.x );
	uvPoint.y=RangeMapFloat( mins.y, maxs.y, 0, 1, point.y );
	return uvPoint;
}

AABB2 AABB2::GetBoxWithin( const Vec2& dimensions, const Vec2& alignment ) const
{
	float maxBoxPosX = maxs.x - dimensions.x;
	float maxBoxPosY = maxs.y - dimensions.y;
	float boxMinPosX = RangeMapFloat( 0, 1, 0, maxBoxPosX, alignment.x );
	float boxMinPosY = RangeMapFloat( 0, 1, 0, maxBoxPosY, alignment.y );
	AABB2 result = AABB2( boxMinPosX, boxMinPosY, boxMinPosX + dimensions.x, boxMinPosY + dimensions.y );
	return result;
}

AABB2 AABB2::GetBoxAtLeft( float fractionOfWidth, float additionalWidth/*=0.f */ ) const
{
	Vec2 dimension = GetDimensions();
	float boxWidth = dimension.x * fractionOfWidth;
	boxWidth += additionalWidth;
	AABB2 result = AABB2( mins.x, mins.y, mins.x + boxWidth, maxs.y );
	return result;
}

AABB2 AABB2::GetBoxAtRight( float fractionOfWidth, float additionalWidth/*=0.f */ ) const
{
	Vec2 dimension = GetDimensions();
	float boxWidth = dimension.x * fractionOfWidth;
	boxWidth += additionalWidth;
	AABB2 result = AABB2( maxs.x - boxWidth, mins.y, maxs.x, maxs.y );
	return result;
}

AABB2 AABB2::GetBoxAtBottom( float fractionOfHeight, float additionalWidth/*=0.f */ ) const
{
	Vec2 dimension = GetDimensions();
	float boxHeight = dimension.y * fractionOfHeight;
	boxHeight += additionalWidth;
	AABB2 result = AABB2( mins.x, mins.y, maxs.x, mins.y + boxHeight );
	return result;
}

AABB2 AABB2::GetBoxAtTop( float fractionOfHeight, float additionalWidth/*=0.f */ ) const
{
	Vec2 dimension = GetDimensions();
	float boxHeight = dimension.y * fractionOfHeight;
	boxHeight += additionalWidth;
	AABB2 result = AABB2( mins.x, maxs.y - boxHeight, maxs.x, maxs.y );
	return result;
}

float AABB2::GetOuterRadius() const
{
	Vec2 dimension = GetDimensions();
	float result = dimension.x / 2 + dimension.y / 2;
	return result;
}

float AABB2::GetInnerRadius() const
{
	Vec2 dimension = GetDimensions();
	float result = dimension.x;
	if( result > dimension.y ){
		result = dimension.y;
	}
	return result;
}

float AABB2::GetWidth()
{
	return ( maxs.x - mins.x );
}

float AABB2::GetHeight()
{
	return ( maxs.y - mins.y );
}

float AABB2::GetDiagonalLength()
{
	float width = GetWidth();
	float height = GetHeight();
	float diagonalLength = sqrtf( ( width * width ) + ( height * height ) );
	return diagonalLength;
}

void AABB2::GetCornerPositions( Vec2* out_fourPoints ) const
{
	out_fourPoints[0] = mins;
	out_fourPoints[1] = Vec2( mins.x, maxs.y );
	out_fourPoints[2] = maxs;
	out_fourPoints[3] = Vec2( maxs.x, mins.y );
}

void AABB2::Translate( const Vec2& translation )
{
	mins+=translation;
	maxs+=translation;
}

void AABB2::SetCenter( const Vec2& newCenter )
{
	Vec2 dimensions=GetDimensions();
	mins=newCenter-(dimensions/2);
	maxs=newCenter+(dimensions/2);
}

void AABB2::SetDimensions( const Vec2& newDimensions )
{
	Vec2 center=GetCenter();
	mins=center-(newDimensions/2);
	maxs=center+(newDimensions/2);
}



void AABB2::StretchToIncludePoint( const Vec2& point )
{
	Vec2 nearestPoint=GetNearestPoint(point);
	if(nearestPoint.x==mins.x){
		mins.x=point.x;
	}
	if(nearestPoint.x==maxs.x){
		maxs.x=point.x;
	}
	if(nearestPoint.y==mins.y){
		mins.y=point.y;
	}
	if( nearestPoint.y==maxs.y ) {
		maxs.y=point.y;
	}
}

void AABB2::MoveToIncludePoint( const Vec2& point )
{
	Vec2 nearestPoint = GetNearestPoint( point );
	Vec2 disp = nearestPoint - point;
	Vec2 center = GetCenter();
	SetCenter( center - disp );
}

void AABB2::filledWithinAABB2( const AABB2 filledAABB2 )
{
	Vec2 direction;
	if(filledAABB2.IsPointInside(mins)&&filledAABB2.IsPointInside(maxs)){return;}
	if(mins.x<filledAABB2.mins.x){
		direction.x=filledAABB2.mins.x-mins.x;
	}
	if(mins.y<filledAABB2.mins.y){
		direction.y=filledAABB2.mins.y-mins.y;
	}
	if( maxs.x>filledAABB2.maxs.x ) {
		direction.x=filledAABB2.maxs.x-maxs.x;
	}
	if( maxs.y>filledAABB2.maxs.y ) {
		direction.y=filledAABB2.maxs.y-maxs.y;
	}
	mins+=direction;
	maxs+=direction;

}

AABB2 AABB2::CarveBoxOffLeft( float fractionOfWidth, float additionalWidhth/*=0.f */ )
{
	AABB2 offBox = GetBoxAtLeft( fractionOfWidth, additionalWidhth );
	mins.x = offBox.maxs.x;
	return offBox;
}

AABB2 AABB2::CarveBoxOffRight( float fractionOfWidth, float additionalWidhth/*=0.f */ )
{
	AABB2 offBox = GetBoxAtRight( fractionOfWidth, additionalWidhth );
	maxs.x = offBox.mins.x;
	return offBox;
}

AABB2 AABB2::CarveBoxOffBottom( float fractionOfHeight, float additionalWidhth/*=0.f */ )
{
	AABB2 offBox = GetBoxAtBottom( fractionOfHeight, additionalWidhth );
	mins.y = offBox.maxs.y;
	return offBox;
}

AABB2 AABB2::CarveBoxOffTop( float fractionOfHeight, float additionalWidhth/*=0.f */ )
{
	AABB2 offBox = GetBoxAtTop( fractionOfHeight, additionalWidhth );
	maxs.y = offBox.mins.y;
	return offBox;
}

void AABB2::AddVertsForAABB2D( std::vector<Vertex_PCU>& vertices, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
{
	Vertex_PCU leftdown		= Vertex_PCU( bound.mins, tintColor, uvAtMins );
	Vertex_PCU rightup		= Vertex_PCU( bound.maxs, tintColor, uvAtMaxs );
	Vertex_PCU leftup		= Vertex_PCU( Vec2( bound.mins.x, bound.maxs.y ), tintColor, Vec2( uvAtMins.x, uvAtMaxs.y ) );
	Vertex_PCU rightdown	= Vertex_PCU( Vec2( bound.maxs.x, bound.mins.y ), tintColor, Vec2( uvAtMaxs.x, uvAtMins.y ) );

	// left up triangle
	vertices.push_back( leftdown );
	vertices.push_back( rightup );
	vertices.push_back( leftup );

	// right down triangle
	vertices.push_back( leftdown );
	vertices.push_back( rightup );
	vertices.push_back( rightdown );
}

void AABB2::operator=( const AABB2& assignFrom )
{
	mins=assignFrom.mins;
	maxs=assignFrom.maxs;
}

bool AABB2::operator==( const AABB2& compareWith ) const
{
	if(mins.operator==(compareWith.mins)&&maxs.operator==(compareWith.maxs)){
		return true;
	}
	else{
		return false;
	}
}
