#pragma once
#include <map>
#include <vector>
#include "Game/Tile.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/vec2.hpp"

class SpriteSheet;
class MapGenStep;


class MapDefinition
{
public:
	MapDefinition(){}
	~MapDefinition(){}
	explicit MapDefinition( XmlElement& tileDefElement );
	const static MapDefinition* GetMapDefinitionByName( std::string mapMame );
	void ParseMapGenSteps( XmlElement& mapGenElement);
	void CreateMapGenStep( XmlElement& mapGenStepElement );
	static void PopulateDefinitionsFromXmlElement( XmlElement& tileDefElement );
	static void PopulateDefinitionsFromXmlFile( const char* filepath );

public:
	static std::map<std::string, MapDefinition> s_definitions;
	std::string					m_name = "";
	int							m_width = 0;
	int							m_edgeThick = 1;
	int							m_height = 0;
	std::string					m_fillTileType = "";
	std::string					m_edgeTileType = "";
	std::vector<MapGenStep*>	m_mapGenSteps;
};

