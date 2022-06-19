#pragma once
#include <math.h>
#include <stack>
#include "Algorithm.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Math/ConvexPoly2.hpp"


#define GJK_CHECK(a,b)  ( DotProduct2D(a,b) > 0 )

Vec2 GetGJK2DSupportPointOfSum( Vec2 direction, const std::vector<Vec2>& shapeA, const std::vector<Vec2>& shapeB )
{
	Vec2 supportA = GetGJK2DSupportPointOfShape( direction, shapeA );
	Vec2 supportB = GetGJK2DSupportPointOfShape( -direction, shapeB );
	return supportA - supportB;
}

Vec2 GetGJK2DSupportPointOfShape( Vec2 direction, const std::vector<Vec2>& shapeA )
{
	float value = DotProduct2D( direction, shapeA[0] );
	Vec2 support = shapeA[0];
	for( int i = 0; i < shapeA.size(); i++ ){
		float tempValue = DotProduct2D( direction, shapeA[i] );
		if( tempValue > value ){
			value = tempValue;
			support = shapeA[i];
		}
	}
	return support;
}

bool GJKIntersectCheck2D( Vec2 direction, const std::vector<Vec2>& shapeA, const std::vector<Vec2>& shapeB )
{
	// initialize
	std::vector<Vec2> simplex;
	return GJKIntersectCheck2DAndGetSimplex( direction, shapeA, shapeB, simplex );
}

bool GJKIntersectCheck2DAndGetSimplex( Vec2 direction, const std::vector<Vec2>& shapeA, const std::vector<Vec2>& shapeB, std::vector<Vec2>& simplex )
{
	simplex.reserve( 3 );
	Vec2 start = GetGJK2DSupportPointOfSum( direction, shapeA, shapeB );
	simplex.push_back( start );
	direction = -direction;
	while( true ) {
		Vec2 point = GetGJK2DSupportPointOfSum( direction, shapeA, shapeB );
		if( GJK_CHECK( direction, point ) ) {
			simplex.push_back( point );
			bool result = GJK2DIfEncloseAndFindNextDirection( simplex, point, direction );
			if( result ) { return true; }
		}
		else {
			return false;
		}
	}
}

bool GJK2DIfEncloseAndFindNextDirection( std::vector<Vec2>& simplex, Vec2 point, Vec2& direction )
{
	if( simplex.size() == 2 ){
		Vec2 pointA = simplex[0];
		Vec2 pointB = simplex[1];
		Vec2 AB = pointB - pointA;
		Vec2 AO = -pointA;	
		Plane2 planeAB = Plane2( AB.GetRotated90Degrees(), pointA );
		if( planeAB.GetSignedDistanceFromPlane( Vec2::ZERO ) > 0 ){
			direction = planeAB.m_normal;
		}
		else {
			direction = -planeAB.m_normal;
		}
		return false;
	}
	else{ /*if( simplex.size() == 3 )*/ 
		Vec2 pointA = simplex[0];
		Vec2 pointB = simplex[1];
		Vec2 pointC = simplex[2];
		Vec2 CA = pointA - pointC;
		Vec2 CB = pointB - pointC;
		Vec3 crossCBA = CrossProduct3D( CB, CA );
		Vec3 v3_CANormal = CrossProduct3D( crossCBA, Vec3( CA ) );
		Vec3 v3_CBNormal = CrossProduct3D( Vec3( CB ), crossCBA );
		Vec3 CANormal = Vec2( v3_CANormal );
		Vec3 CBNormal = Vec2( v3_CBNormal );
		Plane2 planeCA = Plane2( CANormal, pointC );
		Plane2 planeCB = Plane2( CBNormal, pointC );

		if( planeCA.GetSignedDistanceFromPlane( Vec2::ZERO ) > 0 ) {
			direction = planeCA.m_normal;
			simplex.clear();
			simplex.push_back( pointA );
			simplex.push_back( pointC );
			return false;
		}
		else if( planeCB.GetSignedDistanceFromPlane( Vec2::ZERO ) > 0 ) {
			direction = planeCB.m_normal;
			simplex.clear();
			simplex.push_back( pointB );
			simplex.push_back( pointC );
			return false;
		}
		else {
			return true;
		}
	}
	
}

Vec2 GetGJK2DManifold( std::vector<Vec2>& simplex, const std::vector<Vec2>& shapeA, const std::vector<Vec2> shapeB )
{
	return Vec2::ZERO;
// 	GUARANTEE_OR_DIE( simplex.size() == 3, std::string( "simplex size != 3!" ));
// 
// 	bool ifBreak = false;
// 	while( true ) {
// 		ConvexPoly2 polySimplex = ConvexPoly2::MakeConvexPolyFromPointCloud( simplex );
// 
// 		Vec2 closestPoint = polySimplex.GetClosestPointOnEdges( Vec2::ZERO );
// 		Vec2 direction;
// 		if( IsVec2MostlyEqual( closestPoint, Vec2::ZERO ) ){
// 			int edgeIndex = polySimplex.GetEdgeIndexWithPoint( closestPoint );	
// 			if( edgeIndex == -1 ) {
// 				int a = 0;
// 			}
// 			direction = polySimplex.GetEdgeNormal( edgeIndex );
// 		}
// 		else {
// 			direction = closestPoint; 
// 
// 		}
// 		Vec2 supportPoint = GetGJK2DSupportPointOfSum( direction, shapeA, shapeB );
// 		for( int i = 0; i < simplex.size(); i++ ) {
// 			if( IsVec2MostlyEqual( supportPoint, simplex[i] ) ) {
// 				ifBreak = true;
// 				break;
// 			}
// 		}
// 		if( ifBreak ) {
// 			break;
// 		}
// 		simplex.push_back( supportPoint );
// 	}
// 
// 	ConvexPoly2 polyFinalSimplex = ConvexPoly2::MakeConvexPolyFromPointCloud( simplex );
// 	Vec2 closestPoint = polyFinalSimplex.GetClosestPointOnEdges( Vec2::ZERO );
// 	Vec2 direction = closestPoint;
// 	if( IsVec2MostlyEqual( closestPoint, Vec2::ZERO ) ) {
// 		int edgeIndex = polyFinalSimplex.GetEdgeIndexWithPoint( closestPoint );
// 		direction = -polyFinalSimplex.GetEdgeNormal( edgeIndex );
// 	}
// 	else {
// 		direction = closestPoint;
// 
// 	}
// 	return direction;
}
