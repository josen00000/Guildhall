#pragma once
#include<vector>

struct Vec2;
struct Vec3;
struct Vertex_PCU;
struct AABB2;
struct OBB2;
struct IntVec2;
struct FloatRange;
struct Polygon2;
struct Mat44;
struct LineSegment2;
struct Plane2;

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
float	GetSignedDistanceSquared2D( const Vec2& positionA, const Vec2& positionB, const Vec2& reference );
float	GetDistance3D(const Vec3& positionA, const Vec3& positionB);
float	GetDistanceSquared3D(const Vec3& positionA, const Vec3& positionB);
float	GetDistanceXY3D(const Vec3& positionA, const Vec3& positionB);
float	GetDistanceXYSquared3D(const Vec3& positionA, const Vec3& positionB);
float	GetTurnedToward(float currentDegrees, float goalDegrees, float maxDeltaDegrees);//new for asg4. should handle multiple degees for same displacement.
float	GetShortestAngularDisplacement(float inDegree,float targetDegree);
float	GetQuadraticSum( float a, float b );
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
bool IsPointInsideDisc(const Vec2& point, const Vec2& center, float radius);
bool IsPointInSector(const Vec2& point, const Vec2& center, float radius, Vec2 fwdDir, float apertureDegrees );

void PushDiscOutOfDisc2D(Vec2& centerIn, float radiuIn, const Vec2&centerFix, float radiuFix);
void PushDiscsOutOfEachOther2D(Vec2& center1, float radiu1, Vec2& center2, float radiu2);
void PushDiscOutOfPoint2D(Vec2& center, float radiu, const Vec2& point);
void PushDiscOutOfAABB2D(Vec2& center, float radiu,const AABB2& square );

float GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector );
const Vec2 GetProjectedOnto2D( const Vec2& sourceVector, const Vec2& ontoVector );
bool IsPointInForwardSector2D( const Vec2& point, const Vec2& observerPos, float forwardDegrees, float apertureDegrees, float maxDist );
float GetAngleDegreesBetweenVectors2D( const Vec2& vectorA, const Vec2& vectorB );


//
// Lerp and Clamp
// 
float		Interpolate(float a,float b,float fractionOfB);
float		RangeMapFloat(const float inStart,const float inEnd,const float outStart,const float outEnd, const float inValue);
float		ClampRangeMapFloat(const float inStart,const float inEnd,const float outStart,const float outEnd, float inValue);
Vec3		RangeMapVec3( Vec3 inStart, Vec3 inEnd, Vec3 outStart, Vec3 outEnd, Vec3 inValue );
Vec2		RangeMapFromFloatToVec2( float inStart, float inEnd, Vec2 outStart, Vec2 outEnd, float inValue );
Vec2		ClampRangeMapFromFloatToVec2( float inStart, float inEnd, Vec2 outStart, Vec2 outEnd, float inValue );
Vec2		RangeMapPointFromBoxToBox( AABB2 inBox, AABB2 outBox, Vec2 inPoint );
Polygon2	RangeMapPolygonFromBoxToBox( AABB2 inBox, AABB2 outBox, Polygon2 inPolygon );

int			ClampInt( int inMin, int inMax, int inValue );
int			RoundDownToInt(float value);
int			RountToNearestInt(float value); 
int			GetIntFromText(const char* text);
float		ClampFloat(float inMin,float inMax,float inValue);
float		temClampFloat(float inValue, float inMin, float inMax);
float		ClampZeroToOne(float value);
float		Round(float value);
float		GetFloatFromText(const char* text);

//
// product
// 
float DotProduct2D(const Vec2 a, const Vec2 b);
float CrossProduct2D( const Vec2& a, const Vec2& b );
float CrossProduct2D( const Vec2& commonPoint, const Vec2& point1, const Vec2& point2 );
Vec3 CrossProduct3D( const Vec3& a, const Vec3& b );
Vec3 CrossProduct3D( const Vec2& a, const Vec2& b );

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

//
// Almost Check
// 
bool IsFloatMostlyEqual( float a, float b, float epsilon=0.001f );
bool IsVec2MostlyEqual( Vec2 a, Vec2 b, float epsilon=0.001f );
bool IsVec3MostlyEqual( Vec3 a, Vec3 b, float epsilon=0.001f );
bool IsMat44MostlyEqual( Mat44 a, Mat44 b, float epsilon=0.001f );
bool IsPlaneMostlyEqual( Plane2 a, Plane2 b, float epsilon=0.001f );

// Area
float GetAreaOfTriangle( Vec2 a, Vec2 b, Vec2 c );


// Line
Vec2 GetIntersectionPointOfTwoStraightLines( Vec2 pointAInLineA, Vec2 pointBInLineA, Vec2 pointAInLineB, Vec2 pointBInLineB );
Vec2 GetIntersectionPointOfTwoStraightLines( LineSegment2 lineA, LineSegment2 lineB );
bool GetIntersectionPointOfTwoLineSegments( Vec2& point, LineSegment2 lineA, LineSegment2 lineB );
std::pair<Vec2, Vec2> GetIntersectionPointOfLineAndAABB2( LineSegment2 line, AABB2 box );
LineSegment2 GetPerpendicularBisectorOfTwoPoints( Vec2 pointA, Vec2 pointB );

// polygon
LineSegment2 GetAdjacentEdgeOfTwoPolygon( Polygon2 polyA, Polygon2 polyB ); // get first adjacent edge