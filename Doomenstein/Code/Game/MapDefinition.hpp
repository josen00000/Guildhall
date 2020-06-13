#pragma once
#include <string>
#include <map>
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/MaterialsSheet.hpp"

class MapDefinition;
typedef std::map<std::string, MapDefinition> MapDefMap;

enum MapType {
	TILE_MAP,
	NUM_MAP_TYPE
};

class MapDefinition {
public:
	MapDefinition(){}
	~MapDefinition(){}

	static void LoadMapDefinition( const XmlElement& mapElement );
	static MapDefMap s_definitions;

	explicit MapDefinition( const XmlElement& mapElement );

public:
	//MapType	m_type = TILE_MAP;
	std::string m_type = ""; // temp. change to enum later
	int			m_version = 1;
	IntVec2		m_dimensions = IntVec2::ZERO;
	std::map<std::string, std::string> m_lengends;
	std::vector<std::string> m_mapRows;
	//std::
	Vec2	m_playerStartPos = Vec2::ZERO; // temp without using entity
	float	m_playerStartYaw = 0.f;		   // temp without using entity
};