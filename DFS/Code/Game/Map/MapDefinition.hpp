#pragma once
#include <map>
#include <string>
#include "Game/Game.hpp"

class MapGenStep;

typedef std::string TileType;

class MapDefinition {
public:
	MapDefinition(){}
	~MapDefinition(){}
	explicit MapDefinition( XmlElement& mapDefElement );
	const static MapDefinition* GetMapDefByName( std::string mapName );

	static void PopulateDefinitionFromXmlElement( XmlElement& tileDefElement );
	static void PopulateDefinitionsFromXmlFile( const char* filepath );
	void ParseMapGenSteps( const XmlElement& mapGenStepsElement );
	void CreateMapGenStep( const XmlElement& mapGenStepElement );

	// Accessor
	std::string GetName() const { return m_name; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	TileType GetFillType() const { return m_fillTileType; }
	TileType GetEdgeType() const { return m_edgeTileType; }
	TileType GetStartType() const { return m_startTileType; }
	TileType GetEndType() const { return m_endTileType; }
	const std::vector<MapGenStep*>& GetMapGenSteps() const { return m_mapGenSteps; }

private:
	std::string										m_name = "";
	int												m_width = 0;
	int												m_height = 0;
	int												m_edgeThick = 1;
	TileType										m_fillTileType = "";
	TileType										m_edgeTileType = "";
	TileType										m_startTileType = "";
	TileType										m_endTileType = "";
	std::vector<MapGenStep*>						m_mapGenSteps;

public:
	static std::map<std::string, MapDefinition*>	s_definitions;
};