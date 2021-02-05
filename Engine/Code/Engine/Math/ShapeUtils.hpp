#pragma once
#include<vector>
#include "Engine/Math/vec2.hpp"
#include "Engine/Physics/RaycastResult2.hpp"
#include "Engine/Math/ConvexPoly2.hpp"
#include "Engine/Math/ConvexHull2.hpp"
#include "Engine/Core/EngineCommon.hpp"


RaycastResult2 Raycast2DWithConvexPolygons( std::vector<ConvexPoly2> const& polygons, Vec2 startPos, Vec2 forwardDirt );
RaycastResult2 Raycast2DWithConvexPolygon( ConvexPoly2 const& polygon, Vec2 startPos, Vec2 forwardDirt );
RaycastResult2 Raycast2DWithConvexHull( ConvexHull2 const& hull, Vec2 startPos, Vec2 forwardDirt );
RaycastResult2 Raycast2DWithConvexPolygonsWithDist( std::vector<ConvexPoly2> const& polygons, Vec2 startPos, Vec2 forwardDirt );
RaycastResult2 Raycast2DWithConvexPolygonWithDist( ConvexPoly2 const& polygon, Vec2 startPos, Vec2 forwardDirt );
RaycastResult2 Raycast2DWithConvexHullWithDist( ConvexHull2 const& hull, Vec2 startPos, Vec2 forwardDirt );