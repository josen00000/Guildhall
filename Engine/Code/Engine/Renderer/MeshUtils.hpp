#pragma once
#include <vector>
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Capsule2.hpp"

//Append Vertices
void AppendVertsForAABB2D( std::vector<Vertex_PCU>& vertices, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
void AppendVertsForCapsule2D( std::vector<Vertex_PCU>& vertices, const Capsule2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
void AppendVertsForOBB2D( std::vector<Vertex_PCU>& vertices, const OBB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
void AppendVertsForCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float radius, const Rgba8& tintColor );
void AppendVertsForHalfCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float orientationDegrees, float radius, const Rgba8& tintColor );