#pragma once
#include<vector>

struct Vec2;
struct Vec3;
struct Vertex_PCU;
struct AABB2;
struct OBB2;
struct IntVec2;
struct FloatRange;
struct Mat44;

// GJK2D algorithm
Vec2 GetGJK2DSupportPointOfSum( Vec2 direction, const std::vector<Vec2>& shapeA, const std::vector<Vec2>& shapeB );
Vec2 GetGJK2DSupportPointOfShape( Vec2 direction, const std::vector<Vec2>& shapeA );
bool GJKIntersectCheck2D( Vec2 direction, const std::vector<Vec2>& shapeA, const std::vector<Vec2>& shapeB );
bool GJKIntersectCheck2DAndGetSimplex( Vec2 direction, const std::vector<Vec2>& shapeA, const std::vector<Vec2>& shapeB, std::vector<Vec2>& simplex );
bool GJK2DIfEncloseAndFindNextDirection( std::vector<Vec2>& simplex, Vec2 point, Vec2& direction );
Vec2 GetGJK2DManifold( std::vector<Vec2>& simplex, const std::vector<Vec2>& shapeA, const std::vector<Vec2> shapeB);