#pragma once
#include <vector>
#include "Game/Map.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class TileMap : public Map {
public:
	TileMap(){}
	~TileMap(){}
	explicit TileMap( int a );
	static Map* CreateTileMap();

public:
	virtual void RenderMap() const override;
	virtual void UpdateMeshes() override;
	virtual void Update( float deltaSeconds ) override;

public:
	std::vector<Vertex_PCU> m_mesh;
};