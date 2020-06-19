#pragma once
#include <vector>
#include <string>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"

class Map {
public:
	Map(){}
	~Map(){}

	virtual void RenderMap() const = 0;
	virtual void UpdateMeshes() = 0;
	virtual void Update( float delteSeconds ) = 0;
	virtual void PrepareCamera();
public:
	std::string m_name = "";
};