#pragma once
#include <vector>
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Core/EngineCommon.hpp"


//Append Vertices
void AppendVertsForAABB2D( std::vector<Vertex_PCU>& vertices, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
void AppendVertsForAABB3D( std::vector<Vertex_PCU>& vertices, AABB3 box, Rgba8& tintColor );
void AppendVertsForCapsule2D( std::vector<Vertex_PCU>& vertices, const Capsule2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
void AppendVertsForOBB2D( std::vector<Vertex_PCU>& vertices, const OBB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
void AppendVertsForCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float radius, const Rgba8& tintColor );
void AppendVertsForHalfCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float orientationDegrees, float radius, const Rgba8& tintColor );
void AppendVertsForSphere3D( std::vector<Vertex_PCU>& vertices, Vec3 center, float radius, int hCut, int vCut, Rgba8& tintColor );

// Append Indice Vertices
void AppendIndexedVerts( std::vector<Vertex_PCU>& dest, std::vector<uint>& index, const std::vector<Vertex_PCU> source );
void AppendIndexedVertsForAABB2D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
void AppendIndexedVertsForSphere3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, Vec3 center, float radius, int hCut, int vCut, Rgba8& tintColor );
void AppendIndexedVertsForAABB3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, AABB3 box, Rgba8& tintColor );
// void AppendIndexedVertsForCapsule2D( std::vector<Vertex_PCU>& vertices, const Capsule2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
// void AppendIndexedVertsForOBB2D( std::vector<Vertex_PCU>& vertices, const OBB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
// void AppendIndexedVertsForCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float radius, const Rgba8& tintColor );
// void AppendIndexedVertsForHalfCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float orientationDegrees, float radius, const Rgba8& tintColor );

bool IsPointInsideDest( const Vertex_PCU point, const std::vector<Vertex_PCU> dest, uint& index );

Vec2 MakeSphereUVFromPolayDegrees( float theta, float phi );
