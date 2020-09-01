#pragma once
#include <string>
#include <map>
#include "Game/MaterialDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"

class RegionDefinition;
typedef std::map<std::string, RegionDefinition*> RegionTypeMap;

class RegionDefinition {
public:
	RegionDefinition(){}
	~RegionDefinition(){}

	static void LoadMapRegionDefinitions( const XmlElement& mapRegionElement );
	static bool IsRegionTypeValid( std::string regionType );
	static RegionTypeMap s_definitions;
	static RegionDefinition s_defaultRegion;
	explicit RegionDefinition( const XmlElement& MaterialElement );

public:
	// accessor
	bool IsSolid() const { return m_isSolod; }


public:
	bool		m_isSolod = false;
	std::string m_name = "";
	std::map< std::string, MaterialDefinition> m_materials;
};