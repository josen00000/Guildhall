#pragma once
#include<vector>
#include "Game/RegionDefinition.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

struct AABB2;


class Tile {
public:
	Tile(){}
	~Tile(){}
	Tile(const Tile& copyFrom);
	//explicit
	explicit Tile( IntVec2 tileCoords, RegionDefinition* refionDef );
	
public:
	// Accessor
	bool IsSolid() const;
	AABB3 GetBox() const { return m_box; }
	AABB2 GetBox2D() const;
	void GetSideUVs( Vec2& uvAtMax, Vec2& uvAtMins ) const;
	void GetFloorUVs( Vec2& uvAtMax, Vec2& uvAtMins ) const;
	void GetCeilingUVs( Vec2& uvAtMax, Vec2& uvAtMins ) const;
	SpriteSheet* GetTileSpriteSheet() const;

public:	
	IntVec2					m_tileCoords = IntVec2( 0, 0 );
	RegionDefinition*		m_regDef = nullptr;
	AABB3					m_box;
};