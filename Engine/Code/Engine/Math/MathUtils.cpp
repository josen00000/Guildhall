#pragma once
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


#define _USE_MATH_DEFINES 
#ifndef M_PI
#define  M_PI  3.1415926f 
#endif
//
// Forward type declarations
//

extern RenderContext* g_theRenderer;

//
//Angle utilities
//

float ConvertDegreesToRadians( const float degrees ){	
	return (degrees * M_PI / 180.f);
}

float ConvertRadiansToDegrees( const float radians ){
	const float overPie = 180.f / M_PI;
	return (overPie * radians);
}



float CosDegrees(const float degrees ){
	const float t_radians = ConvertDegreesToRadians( degrees );
	return cosf(t_radians);
}

float SinDegrees(const  float degrees ){
	const float t_radians = ConvertDegreesToRadians( degrees );
	return sinf( t_radians );
}

float Atan2Degrees( const float y, const float x ){
	const float t_radians = atan2f( y, x );
	return ConvertRadiansToDegrees( t_radians );
}

float ClampDegressTo360( float degrees )
{
	while( degrees > 360.f ) {
		degrees -= 360.f;
	}
	while( degrees <-0.f ) {
		degrees += 360.f;
	}
	return degrees;

}

Vec2 GetNormalDirectionWithDegrees( const float degrees )
{
	float normalDirectionX = CosDegrees( degrees );
	float normalDirectionY = SinDegrees( degrees );
	return Vec2( normalDirectionX, normalDirectionY );
}

float RangeMapFloat(const float inStart, const float inEnd, const float outStart, const  float outEnd, const float inValue )
{
	float inDisplacement = inValue-inStart;
	float inRange = inEnd - inStart;
	float outRange = outEnd - outStart;
	if( inStart == inEnd ) {
		ERROR_RECOVERABLE( " range map float not vaild! inRange == 0 ");
	}
	float fraction = inDisplacement / inRange;
	float outDisplacement = outRange * fraction;
	float outValue = outStart + outDisplacement;
	return outValue;
}

Vec3 RangeMapVec3( Vec3 inStart, Vec3 inEnd, Vec3 outStart, Vec3 outEnd, Vec3 inValue )
{
	Vec3 result;
	result.x = RangeMapFloat( inStart.x, inEnd.x, outStart.x, outEnd.x, inValue.x );
	result.y = RangeMapFloat( inStart.y, inEnd.y, outStart.y, outEnd.y, inValue.y );
	result.z = RangeMapFloat( inStart.z, inEnd.z, outStart.z, outEnd.z, inValue.z );
	return result;
}

Vec2 RangeMapFromFloatToVec2( float inStart, float inEnd, Vec2 outStart, Vec2 outEnd, float inValue )
{
	Vec2 result;
	result.x = RangeMapFloat( inStart, inEnd, outStart.x, outEnd.x, inValue );
	result.y = RangeMapFloat( inStart, inEnd, outStart.y, outEnd.y, inValue );
	return result;
}

Vec2 RangeMapPointFromBoxToBox( AABB2 inBox, AABB2 outBox, Vec2 inPoint )
{
	Vec2 uvPoint = inBox.GetUVForPoint( inPoint );
	return outBox.GetPointAtUV( uvPoint );
}

Polygon2 RangeMapPolygonFromBoxToBox( AABB2 inBox, AABB2 outBox, Polygon2 inPolygon )
{
	std::vector<Vec2> inPolyPoints;
	std::vector<Vec2> outPolyPoints;
	
	inPolygon.GetAllVerticesInWorld( inPolyPoints );
	for( int i = 0; i < inPolyPoints.size(); i++ ) {
		Vec2 outPoint = RangeMapPointFromBoxToBox( inBox, outBox, inPolyPoints[i] );
		outPolyPoints.push_back( outPoint );
	}

	return Polygon2( outPolyPoints );
}

float ClampFloat( float inMin, float inMax, float inValue )
{
	if(inValue<=inMin){
		return inMin;
	}
	else if(inValue>=inMax){
		return inMax;
	}
	else{
		return inValue;
	}
}

float temClampFloat( float inValue, float inMin, float inMax )
{
	float result = ClampFloat(inMin,inMax,inValue);
	return result;
}

float ClampZeroToOne( float value )
{
	float result=ClampFloat(0,1,value);
	return result;
}

float Round( float value )
{
	return value+1;
}

int ClampInt( int inMin, int inMax, int inValue )
{
	if( inValue <= inMin ) {
		return inMin;
	}
	else if( inValue >= inMax ) {
		return inMax;
	}
	else {
		return inValue;
	}
}

int RoundDownToInt( float value )
{
	int result = (int)floor(value);
	return result;
}

int RountToNearestInt( float value )
{
	return (int)value+1;
}

int GetIntFromText( const char* text )
{
	std::string value = GetStringWithoutSpace( text );
	int result = atoi( value.c_str() );
	return result;
}

float GetFloatFromText( const char* text )
{
	std::string value = GetStringWithoutSpace( text );
	float result = (float)atof(value.c_str());
	return result;
}

float DotProduct2D( const Vec2 a, const Vec2 b )
{
	return a.x * b.x + a.y * b.y;
}

float CrossProduct2D( const Vec2& a, const Vec2& b )
{
	return a.x * b.y - b.x * a.y;
}

float CrossProduct2D( const Vec2& commonPoint, const Vec2& point1, const Vec2& point2 )
{
	Vec2 a = point1 - commonPoint;
	Vec2 b = point2 - commonPoint;
	return CrossProduct2D( a, b );
}

Vec3 CrossProduct3D( const Vec3& a, const Vec3& b )
{
	Vec3 i = Vec3( 1.f, 0.f, 0.f );
	Vec3 j = Vec3( 0.f, 1.f, 0.f );
	Vec3 k = Vec3( 0.f, 0.f, 1.f );

	float detI = ( a.y * b.z ) - ( a.z * b.y );
	float detJ = ( a.x * b.z ) - ( a.z * b.x );
	float detK = ( a.x * b.y ) - ( a.y * b.x );

	Vec3 result = ( detI * i ) - ( detJ * j ) + ( detK * k );
	return result;
}

Vec3 CrossProduct3D( const Vec2& a, const Vec2& b )
{
	Vec3 v3_a = Vec3( a );
	Vec3 v3_b = Vec3( b );
	return CrossProduct3D( v3_a, v3_b );
}

//
//basic 2D & 3D utilities
//

float GetDistance2D( const Vec2& positionA, const Vec2& positionB ){
	const float dis_X=positionB.x-positionA.x;
	const float dis_Y=positionB.y-positionA.y;
	return sqrtf(dis_X*dis_X+dis_Y*dis_Y);
}
float GetDistanceSquared2D( const Vec2& positionA, const Vec2& positionB ){
	const float dis_X=positionB.x-positionA.x;
	const float dis_Y=positionB.y-positionA.y;
	return dis_X*dis_X+dis_Y*dis_Y;
}
float GetDistance3D( const Vec3& positionA, const Vec3& positionB ){
	const float dis_X=positionB.x-positionA.x;
	const float dis_Y=positionB.y-positionA.y;
	const float dis_Z=positionB.z-positionA.z;
	return sqrtf(dis_X*dis_X+dis_Y*dis_Y+dis_Z*dis_Z);

}
float GetDistanceSquared3D( const Vec3& positionA, const Vec3& positionB ){
	const float dis_X=positionB.x-positionA.x;
	const float dis_Y=positionB.y-positionA.y;
	const float dis_Z=positionB.z-positionA.z;
	return dis_X*dis_X+dis_Y*dis_Y+dis_Z*dis_Z ;
}
float GetDistanceXY3D( const Vec3& positionA, const Vec3& positionB ){
	const float dis_X=positionB.x-positionA.x;
	const float dis_Y=positionB.y-positionA.y;
	return sqrtf(dis_X*dis_X+dis_Y*dis_Y);
}
float GetDistanceXYSquared3D( const Vec3& positionA, const Vec3& positionB ){
	const float dis_X=positionB.x-positionA.x;
	const float dis_Y=positionB.y-positionA.y;
	return dis_X*dis_X+dis_Y*dis_Y;
}


float GetTurnedToward( float currentDegrees, float goalDegrees, float maxDeltaDegrees )
{
	
	float displacement=GetShortestAngularDisplacement(currentDegrees,goalDegrees);
	
	float nextDegree=0;
	
	nextDegree=currentDegrees;
	if(abs(displacement)>=maxDeltaDegrees){
		
		if(signbit(displacement)==0){
			nextDegree=currentDegrees+maxDeltaDegrees;
		}
		else{
			nextDegree=currentDegrees-maxDeltaDegrees;
		}
	}
	else{
		nextDegree=goalDegrees;
	}
	while(nextDegree>360){
		nextDegree-=360;
	}
	while(nextDegree<0){
		nextDegree+=360;
	}
	return nextDegree;
}


float GetShortestAngularDisplacement( float inDegree, float targetDegree )
{
	while( targetDegree<inDegree ) {
		targetDegree+=360;
	}
	float displacement=targetDegree-inDegree;
	if( displacement>180 ) {
		displacement=-(360-displacement);
	}
	return displacement;
}

float GetQuadraticSum( float a, float b )
{
	return (( a * a ) + ( b * b ));
}

int GetTaxicabDistance2D( const IntVec2& a, const IntVec2& b )
{
	return abs(b.x - a.x) + abs(b.y - a.y);
}

float SmoothStart2( float t )
{
	float result = t * t;
	return result;
}

float SmoothStart3( float t )
{
	float result = t * t * t;
	return result;
}

float SmoothStart4( float t )
{
	float result = t * t * t * t;
	return result;
}

float SmoothStart5( float t )
{
	float result = t * t * t * t * t;
	return result;
}

float SmoothStop2( float t )
{
	float tempFloat = 1 - t;
	float result = 1 - ( tempFloat * tempFloat );
	return result;
}

float SmoothStop3( float t )
{
	float tempFloat = 1 - t;
	float result = 1 - ( tempFloat * tempFloat * tempFloat );
	return result;
}

float SmoothStop4( float t )
{
	float tempFloat = 1 - t;
	float result = 1 - (tempFloat * tempFloat * tempFloat * tempFloat );
	return result;
}

float SmoothStop5( float t )
{
	float tempFloat = 1 - t;
	float result = 1 - (tempFloat * tempFloat * tempFloat * tempFloat * tempFloat);
	return result;
}

float SmoothStep3( float t )
{
	float result = 3 * ( t * t ) - 2 * ( t * t * t );
	return result;
}

Vec2 GetNearestPointOnCapsule2D( const Vec2& refPos, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius )
{
	// TODO
	// change the way like project to line segment
	// find nearest point in line segment
	// clamp displacement to radius
	// TODO
	
	Vec2 result;
	Vec2 capsuleDirection = capsuleMidEnd - capsuleMidStart;
	Vec2 refPosToStart = refPos - capsuleMidStart;
	float tempDotProduct1 = DotProduct2D( capsuleDirection, refPosToStart );
	if( tempDotProduct1 < 0 ){
		result = GetNearestPointOnDisc2D( refPos, capsuleMidStart, capsuleRadius );
	}
	else{
		Vec2 refPosToEnd = refPos - capsuleMidEnd;
		float tempDotProduct2 = DotProduct2D( capsuleDirection, refPosToEnd );
		if( tempDotProduct2 > 0 ){
			result = GetNearestPointOnDisc2D( refPos, capsuleMidEnd, capsuleRadius );
		}
		else{
			// inside the middle
			Vec2 projectedVec2 = GetProjectedOnto2D( refPosToStart, capsuleDirection );
			Vec2 normalVec2 = refPosToStart - projectedVec2;
			normalVec2.ClampLength( capsuleRadius );
			result = capsuleMidStart + projectedVec2 + normalVec2;
		}
	}
	
	return result;
}

Vec2 GetNearestPointOnLineSegment2D( const Vec2& refPos, const Vec2& start, const Vec2& end )
{
	Vec2 result;
	Vec2 lineDirt = end - start;
	Vec2 refPosToStart = refPos - start;
	float tempDotProduct1 = DotProduct2D( lineDirt, refPosToStart );
	if( tempDotProduct1 < 0 ){
		result = start;
	}
	else{
		Vec2 refPosToEnd = refPos - end;
		float tempDotProduct2 = DotProduct2D( refPosToEnd, lineDirt );
		if( tempDotProduct2 > 0 ){
			result = end;
		}
		else{
			Vec2 projectedVec2 = GetProjectedOnto2D( refPosToStart, lineDirt );
			result = start + projectedVec2;
		}
	}
	
	return result;
}

Vec2 GetNearestPointOnInfiniteLine2D( const Vec2& refPos, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine )
{
	Vec2 result;
	Vec2 lineDirt = anotherPointOnLine - somePointOnLine;
	Vec2 refPosToSomePoint = refPos - somePointOnLine;
	Vec2 projectedVec2 = GetProjectedOnto2D( refPosToSomePoint, lineDirt );
	result = somePointOnLine + projectedVec2;
	return result;
}

Vec2 GetNearestPointOnOBB2D( const Vec2& refPos, const OBB2& box )
{
	Vec2 result = box.GetNearestPoint( refPos );
	return result;
}

Vec2 GetNearestPointOnDisc2D( const Vec2& point,const Vec2& center, float radiu )
{
	Vec2 direction = point-center;
	float dist = direction.GetLength();
	if(dist < radiu){
		return point;
	}
	direction.Normalize();
	Vec2 nearestPoint = center + direction * radiu;
	return nearestPoint;
}

FloatRange GetRangeOnProjectedAxis( int numPoints, const Vec2* points, const Vec2& relativeToPos, const Vec2& axisNormal )
{
	Vec2 projectedOn = axisNormal;
	FloatRange result;
	for(int pointIndex = 0; pointIndex < numPoints; ++pointIndex ){
		Vec2 tempPoint = points[pointIndex];
		Vec2 tempDisp = tempPoint - relativeToPos;
		float tempLength = GetProjectedLength2D( tempDisp, projectedOn );
		if( pointIndex == 0 ){
			result = FloatRange( tempLength ); 
			continue;
		}
		result.AppendFloatRange( tempLength );
	}
	return result;
}

bool DoOBBAndOBBOVerlap2D( const OBB2& boxA, const OBB2& boxB )
{
// 	g_theRenderer->BindTexture(nullptr);
// 	g_theRenderer->DrawLine(Vec2(0,0), Vec2(100,100), 10, Rgba8::WHITE);
// 	float OBBDistanceSquare = ( boxB.m_center - boxA.m_center ).GetLengthSquared();
// 	float boxAOuterRadius = boxA.GetOuterRadius();
// 	float boxAInnerRadius = boxA.GetInnerRadius();
// 	float boxBOuterRadius = boxB.GetOuterRadius();
// 	float boxBInnerRadius = boxB.GetInnerRadius();
// 	float temp = (boxAOuterRadius + boxBOuterRadius) * (boxAOuterRadius + boxBOuterRadius);

// 	if( OBBDistanceSquare > boxAOuterRadius * boxAOuterRadius + boxBOuterRadius * boxBOuterRadius + 2 * boxAOuterRadius * boxBOuterRadius ){
// 		return false;
// 	}
// 
// 	if( OBBDistanceSquare < boxAInnerRadius * boxAInnerRadius + boxBInnerRadius * boxBInnerRadius + 2 * boxAInnerRadius * boxBInnerRadius ) {
// 		return true;
// 	}

	Vec2 boxAVertices[4];
	Vec2 boxBVertices[4];
	boxA.GetCornerPositions( boxAVertices );
	boxB.GetCornerPositions( boxBVertices );
	Vec2 boxAxes[4] = { boxA.m_iBasisNormal, boxA.GetJBasisNormalized(), boxB.m_iBasisNormal,boxB.GetJBasisNormalized() };
	for( int axisIndex = 0; axisIndex < 4; axisIndex++  ){
		Vec2 tempAxis = boxAxes[axisIndex];
		//Vec2 tempAxisNormal = Vec2( -tempAxis.y, tempAxis.x );
		Vec2 tempPos = Vec2(0,0);
		FloatRange boxAFloatRange = GetRangeOnProjectedAxis( 4, boxAVertices, tempPos, tempAxis );
		FloatRange boxBFloatRange = GetRangeOnProjectedAxis( 4, boxBVertices, tempPos, tempAxis );
		std::string boxAFloatRangeInString = " A = " + boxAFloatRange.GetAsString();
		std::string boxBFloatRangeInString = " B = " + boxBFloatRange.GetAsString();
		if( !boxAFloatRange.DoesOverlap( boxBFloatRange )){
			//g_theRenderer->DrawLine(Vec2::ZERO, Vec2::ZERO + tempAxisNormal * 10, 1, Rgba8::WHITE );
			return false;
		}
	}
	return true; 
}

bool IsPointInsideCapsule2D( const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius )
{
	Vec2 nearestPoint = GetNearestPointOnLineSegment2D( point, capsuleMidStart, capsuleMidEnd );
	Vec2 tempDisp = point - nearestPoint;
	float tempLength = tempDisp.GetLength();
	if( tempLength <= capsuleRadius ){
		return true;
	}
	else{
		return false;
	}
}

bool IsPointInsideOBB2D( const Vec2& point, const OBB2& box )
{
	bool result = box.IsPointInside( point );
	return result;
}

bool IsPointInsideAABB2D( const Vec2& point, const AABB2& box )
{
	if(( point.x >= box.mins.x && point.x <= box.maxs.x ) && ( point.y >= box.mins.y && point.y <= box.maxs.y )){
		return true;
	}
	else{
		return false;
	}
}

Vec2 GetNearestPointOnAABB2D( const Vec2& point, const AABB2& square )
{
	Vec2 nearestPoint = square.GetNearestPoint(point);
	return nearestPoint;
}

void PushDiscOutOfDisc2D( Vec2& pushedCenter, float pushedradius, const Vec2& pushCenter, float pushRadius )
{
	Vec2 direction = pushedCenter - pushCenter;
	float dis = direction.NormalizeAndGetPreviousLength();
	if( dis > ( pushedradius + pushRadius ) ) {
		return;
	}
	float pushDis = pushedradius + pushRadius - dis;
	pushedCenter += direction * pushDis;
}

/*
void PushDiscOutOfDisc2D( Vec2& centerIn, float radiuIn, const Vec2& centerFix, float radiuFix )
{
	Vec2 direction=centerIn-centerFix;
	float dis=direction.NormalizeAndGetPreviousLength();
	if(dis > (radiuIn + radiuFix)){
		return;
	}
	float pushDis=radiuIn+radiuFix-dis;
	centerIn+=direction*pushDis;
}*/

void PushDiscsOutOfEachOther2D( Vec2& center1, float radiu1, Vec2& center2, float radiu2 )
{
	Vec2 direction=center2-center1;
	float dis=direction.NormalizeAndGetPreviousLength();
	if(dis > (radiu1 + radiu2)){return ;}
	float pushDis=radiu1+radiu2-dis;
	center1-=(direction*pushDis/2);
	center2+=(direction*pushDis/2);
}

void PushDiscOutOfPoint2D( Vec2& center, float radiu, const Vec2& point )
{
	Vec2 direction=center-point;
	float dis=direction.NormalizeAndGetPreviousLength();
	if(dis > radiu){
		return;
	}
	float pushDis=radiu-dis;
	center= center + direction * pushDis;
	
}



void PushDiscOutOfAABB2D( Vec2& center, float radiu, const AABB2& square )
{
	Vec2 point = square.GetNearestPoint(center);
	Vec2 displacement = point-center;
	float dist = displacement.GetLength();
	if( dist < radiu ) {
		PushDiscOutOfPoint2D(center,radiu,point);
	}
}

bool IsPointInSector( const Vec2& point, const Vec2& center, float radius, Vec2 fwdDir, float apertureDegrees )
{
	float distance = GetDistance2D(point, center);
	if(distance > radius){return false ;}

	Vec2 pointInCenterCoords = point - center;
	float thetaDegrees = pointInCenterCoords.GetAngleDegrees();
	float fwdDegrees = fwdDir.GetAngleDegrees();
	if(fabsf(thetaDegrees - fwdDegrees) < apertureDegrees){
		return true;
	}
	else{
		return false;
	}

}

bool IsPointInsideDisc( const Vec2& point, const Vec2& center, float radius )
{
	float distance = GetDistance2D( point, center );
	if( distance > radius ) { return false; }
	else{ return true;	}
}

float GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
	float product = DotProduct2D(sourceVector, ontoVector);
	float ontoLength = ontoVector.GetLength();
	float result = product / ontoLength;
	return result;
}

const Vec2 GetProjectedOnto2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
	Vec2 normalizedOntoVector = ontoVector.GetNormalized();
	float length = GetProjectedLength2D(sourceVector,ontoVector);
	Vec2 result = normalizedOntoVector * length;
	return result;

}

bool IsPointInForwardSector2D( const Vec2& point, const Vec2& observerPos, float forwardDegrees, float apertureDegrees, float maxDist )
{

	Vec2 disp = point - observerPos;
	float dist = disp.GetLength();
	if(dist > maxDist){return false ;}
	//disp.SetLength(maxDist);
	float pointDegrees = disp.GetAngleDegrees();
	float absValue = fabsf(GetShortestAngularDisplacement(pointDegrees, forwardDegrees));
	
	if(absValue < apertureDegrees/2){
		return true;
	}
	else{
		return false;
	}
}

float GetAngleDegreesBetweenVectors2D( const Vec2& vectorA, const Vec2& vectorB )
{
	float product = DotProduct2D(vectorA, vectorB);
	float lengthA = vectorA.GetLength();
	float lengthB = vectorB.GetLength();
	float AngleDegreesInCos = product / (lengthA * lengthB);
	float angleDegrees = (float)(acos(AngleDegreesInCos) * 180.f / M_PI);
	return angleDegrees;
}

float Interpolate( float a, float b, float fractionOfB )
{
	return a+fractionOfB*(b-a);
}

//
//Geometric query utilities
//

const bool DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB ){
	const float t_DisSqrt=GetDistanceSquared2D(centerA,centerB);
	if(t_DisSqrt<=radiusA*radiusA+radiusB*radiusB+2*radiusA*radiusB)return true;
	else return false;
	
}
const bool DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB ){
	const float t_DisSqrt=GetDistanceSquared3D(centerA,centerB);
	if(t_DisSqrt<radiusB*radiusB+radiusA*radiusA+2*radiusA*radiusB)return true;
	else return false;
}

const bool DoLineOverlap( const Vec2& centerA, float radiusA, float lineParameterA, float lineParameterB, float lineParameterC )
{
	// unfinished
	const float distance=(lineParameterA*centerA.x+lineParameterB*centerA.y+lineParameterC)/sqrtf(lineParameterA*lineParameterA+lineParameterB*lineParameterB);
	if(distance<radiusA){
		return true;
	}
	else{
		return false;
	}

}

//
//Transform utilities
//
const Vec2 TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation ){
	
	const Vec2 tem_Vec2=position;
	Vec2 scaled_Vec2=tem_Vec2.operator *(uniformScale);
	scaled_Vec2.RotateDegrees(rotationDegrees);
	Vec2 transformed_Vec2=scaled_Vec2.operator+(translation);
	return  Vec2(transformed_Vec2);
}

const Vec2 TransformPosition2D( const Vec2& position, const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation )
{
	float resultX = iBasis.x * position.x + jBasis.x * position.y + translation.x;
	float resultY = iBasis.y * position.x + jBasis.y * position.y + translation.y;
	Vec2 result = Vec2(resultX, resultY);
	return result;
}

const Vec3 TransformPosition3DXY( const Vec3& position, float scaleXY, float zRotationDegrees, const Vec2& translationXY ){
	const Vec2 tem_Vec2=Vec2(position.x,position.y);
	Vec2 scaled_Vec2=tem_Vec2.operator *( scaleXY );
	scaled_Vec2.RotateDegrees( zRotationDegrees );
	Vec2 transformed_Vec2=scaled_Vec2.operator+( translationXY );
	return  Vec3(transformed_Vec2,position.z);
}


const Vec3 TransformPosition3DXY( const Vec3& position, const Vec2& iBasisXY, const Vec2& jBasisXY, const Vec2& translationXY )
{
	float resultX = iBasisXY.x * position.x + jBasisXY.x * position.y + translationXY.x;
	float resultY = iBasisXY.y * position.x + jBasisXY.y * position.y + translationXY.y;
	Vec3 result = Vec3( resultX, resultY, position.z );
	return result;
}

const std::vector<Vertex_PCU> TransformVertexArray( const std::vector <Vertex_PCU>& vertices, float scale, float rot, Vec2 trans )
{
	float cos=CosDegrees(rot)*scale;
	float sin=SinDegrees(rot)*scale;
	Vec2 i(cos,sin);
	Vec2 j(-sin,cos);
	std::vector <Vertex_PCU> result;
	for(int vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++){
		Vec2 newPos=Vec2(vertices[vertexIndex].m_pos.x*i+vertices[vertexIndex].m_pos.y*j+trans);
		result.push_back(Vertex_PCU(Vec3(newPos,0),vertices[vertexIndex].m_color,vertices[vertexIndex].m_uvTexCoords));
	}
	return result;
}

IntVec2 TransformFromWorldPosToTilePos( const Vec2 worldPos )
{
	int temWorldPosX = RoundDownToInt( worldPos.x);
	int temWorldPosY = RoundDownToInt( worldPos.y);
	IntVec2 tilePos = IntVec2(temWorldPosX, temWorldPosY);
	return tilePos;
}

bool IsFloatMostlyEqual( float a, float b, float epsilon/*=0.001f */ )
{
	float diff = a - b;
	if( diff <= epsilon && diff >= -epsilon ) {
		return true;
	}
	else{
		return false;
	}
}

bool IsVec2MostlyEqual( Vec2 a, Vec2 b, float epsilon/*=0.001f */ )
{
	if( IsFloatMostlyEqual( a.x, b.x, epsilon ) && IsFloatMostlyEqual( a.y, b.y, epsilon ) ){
		return true;
	}
	else{
		return false;
	}
}

bool IsVec3MostlyEqual( Vec3 a, Vec3 b, float epsilon/*=0.001f */ )
{
	if( IsFloatMostlyEqual( a.x, b.x, epsilon ) && IsFloatMostlyEqual( a.y, b.y, epsilon ) && IsFloatMostlyEqual( a.z, b.z, epsilon ) ) {
		return true;
	}
	else {
		return false;
	}
}

bool IsMat44MostlyEqual( Mat44 a, Mat44 b, float epsilon/*=0.01f */ )
{
	for( int i = 0; i < 16; i++ ){
		if( !IsFloatMostlyEqual( a.m_values[i], b.m_values[i], epsilon ) ){
			return false;
		}
	}
	return true;
}

float GetAreaOfTriangle( Vec2 a, Vec2 b, Vec2 c )
{
	Vec2 ab = b - a;
	Vec2 ac = c - a;
	float area = Vec2::CrossProduct( ab, ac ) * 0.5f;
	if( area < 0 ){
		area = - area;
	}
	return area;
}

Vec2 GetIntersectionPointOfTwoLines( Vec2 pointAInLineA, Vec2 pointBInLineA, Vec2 pointAInLineB, Vec2 pointBInLineB )
{
	float a1 = pointBInLineA.y - pointAInLineA.y;
	float b1 = pointAInLineA.x - pointBInLineA.x;
	float c1 = a1 * pointAInLineA.x + b1 * pointAInLineA.y;

	float a2 = pointBInLineB.y - pointAInLineB.y;
	float b2 = pointAInLineB.x - pointBInLineB.x;
	float c2 = a2 * pointAInLineB.x + b2 * pointAInLineB.y;

	float determinant = a1 * b2 - a2 * b1;

	if( determinant == 0 ) {
		return Vec2( NAN, NAN );
	}
	else {
		Vec2 result;
		result.x = ( ( b2 * c1 ) - ( b1 * c2) ) / determinant;
		result.y = ( ( a1 * c2 ) - ( a2 * c1) ) / determinant;
		return result;
	}
}

Vec2 GetIntersectionPointOfTwoLines( LineSegment2 lineA, LineSegment2 lineB )
{
	return GetIntersectionPointOfTwoLines( lineA.GetStartPos(), lineA.GetEndPos(), lineB.GetStartPos(), lineB.GetEndPos() );
}

std::vector<Vec2> GetIntersectionPointOfLineAndAABB2( LineSegment2 line, AABB2 box )
{
	LineSegment2 boxBottom	= LineSegment2( box.mins, Vec2( box.maxs.x, box.mins.y ) );
	LineSegment2 boxTop		= LineSegment2( Vec2( box.mins.x, box.maxs.y ), box.maxs );
	LineSegment2 boxLeft	= LineSegment2( box.mins, Vec2( box.mins.x, box.maxs.y ) );
	LineSegment2 boxRight	= LineSegment2( Vec2( box.maxs.x, box.mins.y ), box.maxs );

	std::vector<Vec2> result;
	if( line.GetStartPos().y == line.GetEndPos().y ) {
		Vec2 pointA = GetIntersectionPointOfTwoLines( boxLeft, line );
		Vec2 pointB = GetIntersectionPointOfTwoLines( boxRight, line );
		result.push_back( pointA );
		result.push_back( pointB );
	}
	else if( line.GetStartPos().x == line.GetEndPos().x ) {
		Vec2 pointA = GetIntersectionPointOfTwoLines( boxBottom, line );
		Vec2 pointB = GetIntersectionPointOfTwoLines( boxTop, line );
		result.push_back( pointA );
		result.push_back( pointB );
	}
	else {
		Vec2 pointA = GetIntersectionPointOfTwoLines( boxBottom, line );
		Vec2 pointB = GetIntersectionPointOfTwoLines( boxTop, line );
		Vec2 pointC = GetIntersectionPointOfTwoLines( boxLeft, line );
		Vec2 pointD = GetIntersectionPointOfTwoLines( boxRight, line );
		if( boxBottom.IsPointMostlyInEdge( pointA ) ) {
			result.push_back( pointA );
		}
		if( boxTop.IsPointMostlyInEdge( pointB ) ) {
			result.push_back( pointB );
		}
		if( boxLeft.IsPointMostlyInEdge( pointC ) ) {
			result.push_back( pointC );
		}
		if( boxRight.IsPointMostlyInEdge( pointD ) ) {
			result.push_back( pointD );
		}
		if( result.size() > 2 ) {
			ERROR_RECOVERABLE( " should not have more than two intersect point!" );
		}
	}
	return result;
}

