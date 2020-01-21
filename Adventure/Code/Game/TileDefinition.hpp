#pragma once
#include <vector>
#include <string>
#include <map>
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/XmlUtils.hpp"

class SpriteSheet;

typedef	std::map<std::string, TileDefinition*> tileTypeDefMap;
typedef tileTypeDefMap::const_iterator tileTypeDefMapIterator;
class TileDefinition
{
public:
	TileDefinition(){}
	~TileDefinition(){}
	explicit TileDefinition(const XmlElement& tileDefElement);
	void GetSpriteUVs(Vec2& uvAtMins, Vec2& uvAtMaxs) const;
	bool GetTileAllowsSight() const;
	bool GetTileAllowsWalk() const;
	bool GetTileAllowsSwim() const;
	bool GetTileAllowsFly() const;
	Rgba8 GetTintColor() const;
	static void ClearDefinitions();
	static void PopulateDefinitionsFromXmlElement(const XmlElement& tileDefElement);
	static void ParseSpriteSheetFromXmlElement( const XmlElement& spriteElement );

public:
	static tileTypeDefMap s_definitions;
	static std::string s_spriteFilePath;
	static IntVec2 s_spriteLayout;
	std::string m_name = "";
	IntVec2 m_spriteCoords = IntVec2();
	Rgba8 m_tintColor = Rgba8::WHITE;
	Rgba8 m_fromImageColor = Rgba8::WHITE;
	bool m_allowsSight	= false;
	bool m_allowsWalk	= false;
	bool m_allowsSwim	= false;
	bool m_allowsFly	= false;
	//sprite sheet uv
	Vec2 m_uvAtMins = Vec2::ZERO;
	Vec2 m_uvAtMaxs = Vec2::ONE;
};

