#pragma once
#include<vector>
struct Vec2;
struct Vec3;
struct Vertex_PCU;
struct AABB2;
struct OBB2;
struct IntVec2;
struct FloatRange;



//
//Angle utilities
//
float	ConvertRadiansToDegrees( const float radians );
float	ConvertDegreesToRadians( const float degrees );
float	CosDegrees( const float degrees );
float	SinDegrees( const float degrees );
float	Atan2Degrees( const float y,const float x );
float	ClampDegressTo360( float degrees );
Vec2	GetNormalDirectionWithDegrees( const float degrees );


//
//basic 2D & 3D utilities
//
float	GetDistance2D(const Vec2& positionA, const Vec2& positionB);
float	GetDistanceSquared2D(const Vec2& positionA, const Vec2& positionB);
float	GetDistance3D(const Vec3& positionA, const Vec3& positionB);
float	GetDistanceSquared3D(const Vec3& positionA, const Vec3& positionB);
float	GetDistanceXY3D(const Vec3& positionA, const Vec3& positionB);
float	GetDistanceXYSquared3D(const Vec3& positionA, const Vec3& positionB);
float	GetTurnedToward(float currentDegrees, float goalDegrees, float maxDeltaDegrees);//new for asg4. should handle multiple degees for same displacement.
float	GetShortestAngularDisplacement(float inDegree,float targetDegree);
int		GetTaxicabDistance2D(const IntVec2& a, const IntVec2& b);
//Vec2	GetPerpendicularVec( const Vec2& vec );

//
//Smooth function
// 
float SmoothStart2( float t );
float SmoothStart3( float t );
float SmoothStart4( float t );
float SmoothStart5( float t );
float SmoothStop2( float t );
float SmoothStop3( float t );
float SmoothStop4( float t );
float SmoothStop5( float t );
float SmoothStep3( float t );

//
//Geometric utilities
// 
Vec2 GetNearestPointOnCapsule2D( const Vec2& refPos, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius );
Vec2 GetNearestPointOnLineSegment2D( const Vec2& refPos, const Vec2& start, const Vec2& end );
Vec2 GetNearestPointOnInfiniteLine2D( const Vec2& refPos, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine );
Vec2 GetNearestPointOnOBB2D( const Vec2& refPos, const OBB2& box );
Vec2 GetNearestPointOnAABB2D(const Vec2& point,const AABB2& square);
Vec2 GetNearestPointOnDisc2D(const Vec2& point,const Vec2& center, float radiu);
FloatRange GetRangeOnProjectedAxis( int numPoints, const Vec2* points, const Vec2& relativeToPos, const Vec2& axisNormal );
bool DoOBBAndOBBOVerlap2D( const OBB2& boxA, const OBB2& boxB );

bool IsPointInsideCapsule2D( const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius );
bool IsPointInsideOBB2D( const Vec2& point, const OBB2& box );
bool IsPointInsideAABB2D( const Vec2& point, const AABB2& box );
bool IsPointInSector(const Vec2& point, const Vec2& center, float radius, Vec2 fwdDir, float apertureDegrees );
bool IsPointInDisc(const Vec2& point, const Vec2& center, float radius);




void PushDiscOutOfDisc2D(Vec2& centerIn, float radiuIn, const Vec2&centerFix, float radiuFix);
void PushDiscsOutOfEachOther2D(Vec2& center1, float radiu1, Vec2& center2, float radiu2);
void PushDiscOutOfPoint2D(Vec2& center, float radiu, const Vec2& point);
void PushDiscOutOfAABB2D(Vec2& center, float radiu,const AABB2& square );

float GetProjectedLength2D(const Vec2& sourceVector, const Vec2& ontoVector);
const Vec2 GetProjectedOnto2D(const Vec2& sourceVector, const Vec2& ontoVector);
bool IsPointInForwardSector2D(const Vec2& point, const Vec2& observerPos, float forwardDegrees, float apertureDegrees, float maxDist);
float GetAngleDegreesBetweenVectors2D(const Vec2& vectorA, const Vec2& vectorB);



//
// Lerp and Clamp
// 
float Interpolate(float a,float b,float fractionOfB);
float RangeMapFloat(const float inStart,const float inEnd,const float outStart,const float outEnd, const float inValue);
float ClampFloat(float inMin,float inMax,float inValue);
float temClampFloat(float inValue, float inMin, float inMax);
float ClampZeroToOne(float value);
float Round(float value);
int RoundDownToInt(float value);
int RountToNearestInt(float value); 
int GetIntFromText(const char* text);
float GetFloatFromText(const char* text);

//
//
// 
float DotProduct2D(const Vec2 a, const Vec2 b);
float CrossProduct2D( const Vec2& a, const Vec2& b );
float CrossProduct2D( const Vec2& commonPoint, const Vec2& point1, const Vec2& point2 );

//
//Geometric query utilities
//

const bool DoDiscsOverlap(const Vec2&centerA,float radiusA,const Vec2& centerB, float radiusB);
const bool DoSpheresOverlap(const Vec3& centerA, float radiusA, const Vec3& centerB,float radiusB);
const bool DoLineOverlap(const Vec2& centerA, float radiusA,float lineParameterA,float lineParameterB, float lineParameterC);
//
//Transform utilities
//
const Vec2 TransformPosition2D(const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation);
const Vec2 TransformPosition2D(const Vec2& position, const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation);
const Vec3 TransformPosition3DXY(const Vec3& position, float scaleXY, float zRotationDegrees, const Vec2& translationXY);
const Vec3 TransformPosition3DXY(const Vec3& position, const Vec2& iBasisXY, const Vec2& jBasisXY,  const Vec2& translationXY);
const std::vector<Vertex_PCU> TransformVertexArray(const std::vector<Vertex_PCU>& vertices,float scale, float rot, Vec2 trans); //need to refine. not return value but change directly
// void transformVertexArray(int number, Vertex_PCU* vertexArray); only for vertices exists.

//Pos
IntVec2 TransformFromWorldPosToTilePos(const Vec2 worldPos);