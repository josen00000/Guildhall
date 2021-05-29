#pragma once
#include <string>
#include <map>
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/MaterialsSheet.hpp"

class MaterialDefinition;
typedef std::map<std::string, MaterialDefinition> MaterialTypeMap;

class MaterialDefinition {
public:
	MaterialDefinition(){}
	~MaterialDefinition(){}

	static void LoadMapMaterialDefinitions( const XmlElement& mapMaterialElement );
	static MaterialTypeMap s_definitions;
	static MaterialsSheet s_sheet;
	static MaterialDefinition s_defaultMaterial;

	explicit MaterialDefinition( const XmlElement& MaterialElement );

	//
	bool isSpriteCoordsValid( IntVec2 spriteCoords );

public:
	std::string m_name = "";
	std::string m_sheetName = "";
	IntVec2		m_spriteCoords = IntVec2( 0, 0 );
};