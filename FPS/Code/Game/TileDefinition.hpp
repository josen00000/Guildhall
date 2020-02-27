#pragma once
#include <vector>
#include "Game/Tile.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/vec2.hpp"
typedef int TileIndex;


class TileDefinition
{
public:
	TileDefinition(){}
	~TileDefinition(){}
	explicit TileDefinition(Rgba8 tintColor, bool isSolid , const Vec2 uvAtMins, const Vec2 uvAtMaxs);
	//void AddTileDefinition( Rgba8 tintColor, bool isSolid, const Vec2 uvAtMins, const Vec2 uvAtMaxs);
	bool GetTileIsSolid() const;
	void GetSpriteUVs(Vec2& uvAtMins, Vec2& uvAtMaxs) const;
	Rgba8 GetTintColor() const;
	static void ClearDefinitions();
	static std::vector<TileDefinition> s_definitions;

public:

private:
	Rgba8 m_tintColor = Rgba8(0,0,0,0);
	bool m_isSolid = false;
	Vec2 m_uvAtMins = Vec2::ZERO;
	Vec2 m_uvAtMaxs = Vec2::ONE;
	//sprite sheet uv

};

