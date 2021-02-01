#pragma once
#include<vector>
#include "Engine/Math/vec2.hpp"
#include "Engine/Physics/RaycastResult2.hpp"
#include "Engine/Math/ConvexPoly2.hpp"

RaycastResult2 Raycast2DWithPolygons( Vec2 startPos, Vec2 forwardDirt, std::vector<ConvexPoly2*> const& polygons );
RaycastResult2 Raycast2DWithPolygon( Vec2 startPos, Vec2 forwardDirt, ConvexPoly2 const& polygons );