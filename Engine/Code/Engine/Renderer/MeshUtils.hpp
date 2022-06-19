#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/Cone3.hpp"
#include "Engine/Math/Cylinder3.hpp"
#include "Engine/Math/ConvexPoly2.hpp"


struct OBB3;

//Append Vertices for Vertex_PCU
void AppendVertsForAABB2D( std::vector<Vertex_PCU>& vertices, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins=Vec2::ZERO, const Vec2& uvAtMaxs=Vec2::ONE );
void AppendVertsForConvexPoly2D( std::vector<Vertex_PCU>& vertices, const ConvexPoly2& poly, const Rgba8& tintColor );
void AppendVertsForPolygon2D( std::vector<Vertex_PCU>& vertices, const ConvexPoly2& poly, const Rgba8& tintColor ); // different data structure
void AppendVertsForAABB2DWithHeight( std::vector<Vertex_PCU>& vertices, const AABB2& bound, float height, const Rgba8& tintColor, const Vec2& uvAtMins=Vec2::ZERO, const Vec2& uvAtMaxs=Vec2::ONE );
void AppendVertsForAABB3D( std::vector<Vertex_PCU>& vertices, AABB3 box, Rgba8& tintColor, Convention convention=X_RIGHT_Y_UP_Z_BACKWARD );
void AppendVertsForCapsule2D( std::vector<Vertex_PCU>& vertices, const Capsule2& bound, const Rgba8& tintColor );
void AppendVertsForLineSegment2D( std::vector<Vertex_PCU>& vertices, const LineSegment2& line, float thickness, const Rgba8& tintColor );
void AppendVertsForCapsule2DWithHeight( std::vector<Vertex_PCU>& vertices, const Capsule2& bound, const Rgba8& tintColor );
void AppendVertsForOBB2D( std::vector<Vertex_PCU>& vertices, const OBB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins=Vec2::ZERO, const Vec2& uvAtMaxs=Vec2::ONE );
void AppendVertsForOBB2DWithHeight( std::vector<Vertex_PCU>& vertices, const OBB2& bound, float height, const Rgba8& tintColor, const Vec2& uvAtMins=Vec2::ZERO, const Vec2& uvAtMaxs=Vec2::ONE );
void AppendVertsForOBB2DWithHeight( std::vector<Vertex_PCU>& vertices, const OBB2& bound, float height, const Rgba8& startColor, const Rgba8& endColor, const Vec2& uvAtMins=Vec2::ZERO, const Vec2& uvAtMaxs=Vec2::ONE );
//void AppendVertsForOBB3D( std::vector<Vertex_PCU>& vertices, const OBB3& bound, const Rgba8& tintColor );
void AppendVertsForCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float radius, const Rgba8& tintColor );
void AppendVertsForHalfCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float orientationDegrees, float radius, const Rgba8& tintColor );
void AppendVertsForSphere3D( std::vector<Vertex_PCU>& vertices, Vec3 center, float radius, int hCut, int vCut, Rgba8& tintColor );
void AppendVertsForCubeSPhere3D( std::vector<Vertex_PCU>& vertices, const AABB3& cube, int level );
void AppendVertsForCylinder3D( std::vector<Vertex_PCU>& vertices, Cylinder3 cylinder, int level, Rgba8& tintColor );
void AppendVertsForCylinder3D( std::vector<Vertex_PCU>& vertices, Cylinder3 cylinder, int level, const Rgba8& startTintColor, const Rgba8& endTintColor );
void AppendVertsForCone3D( std::vector<Vertex_PCU>& vertices, Cone3 cone, int level, const Rgba8& startTintColor, const Rgba8& endTintColor );

// Append Vertices for Vertex_PCUTBN
void AppendTBNVertsForAABB2D( std::vector<Vertex_PCUTBN>& vertices, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
void AppendTBNVertsForAABB3D( std::vector<Vertex_PCUTBN>& vertices, AABB3 box, Rgba8& tintColor );
void AppendTBNVertsForSphere3D( std::vector<Vertex_PCUTBN>& vertices, Vec3 center, float radius, int hCut, int vCut, Rgba8& tintColor );

// Append Indice Vertices
void AppendIndexedVerts( std::vector<Vertex_PCU>& dest, std::vector<uint>& index, const std::vector<Vertex_PCU> source );
void AppendIndexedVertsForAABB2D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
void AppendIndexedVertsForConvexPoly2D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, const ConvexPoly2& poly, const Rgba8& tintColor );
void ManuallyAppendIndexedVertsForConvexPoly2D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, const ConvexPoly2& poly, const Rgba8& tintColor );
void AppendIndexedVertsForSphere3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, Vec3 center, float radius, int hCut, int vCut, Rgba8& tintColor );
void AppendIndexedVertsForAABB3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, AABB3 box, Rgba8& tintColor, Convention convention=X_RIGHT_Y_UP_Z_BACKWARD );
void AppendIndexedVertsForCubeSphere3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, const AABB3& cube, int level );
void AppendIndexedVertsForCylinder3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, Cylinder3 cylinder, int level, Rgba8& tintColor );
void AppendIndexedVertsForCylinder3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, Cylinder3 cylinder, int level, const Rgba8& startTintColor, const Rgba8& endTintColor );
void AppendIndexedVertsForCone3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, Cone3 cone, int level, const Rgba8& startTintColor, const Rgba8& endTintColor );
void AppendIndexedVertsForCone3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, Cone3 cone, int level, const Rgba8& tintColor );

// void AppendIndexedVertsForCapsule2D( std::vector<Vertex_PCU>& vertices, const Capsule2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
// void AppendIndexedVertsForOBB2D( std::vector<Vertex_PCU>& vertices, const OBB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
// void AppendIndexedVertsForCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float radius, const Rgba8& tintColor );
// void AppendIndexedVertsForHalfCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float orientationDegrees, float radius, const Rgba8& tintColor );

// Append indice Vertices for Vertex_PCUTBN
void AppendIndexedTBNVerts( std::vector<Vertex_PCUTBN>& dest, std::vector<uint>& index, const std::vector<Vertex_PCUTBN> source );
void AppendIndexedTBNVertsForAABB2D( std::vector<Vertex_PCUTBN>& vertices, std::vector<uint>& index, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
void AppendIndexedTBNVertsForAABB3D( std::vector<Vertex_PCUTBN>& vertices, std::vector<uint>& index, AABB3 box, Rgba8& tintColor );
void AppendIndexedTBNVertsForSphere3D( std::vector<Vertex_PCUTBN>& vertices, std::vector<uint>& index, Vec3 center, float radius, int hCut, int vCut, Rgba8& tintColor );


// help function
bool IsPointInsideDest( const Vertex_PCU point, const std::vector<Vertex_PCU> dest, uint& index );
bool IsTBNPointInsideDest( const Vertex_PCUTBN& point, const std::vector<Vertex_PCUTBN> & dest, int& index );
Vec2 MakeSphereUVFromPolarDegrees( float theta, float phi );
Vec3 MakeSphereTangentFromPolarDegrees( float theta );
Vec3 MakeSphereBitangentFromPolarDegrees( float theta, float phi );
void AppendTesselateIndexedVerts( std::vector<Vertex_PCU>& dest, std::vector<uint>& index, const std::vector<Vertex_PCU> source );