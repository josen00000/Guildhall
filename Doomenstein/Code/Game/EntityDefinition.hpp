#pragma once
#include <string>
#include <map>
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/MaterialsSheet.hpp"

class EntityDefinition;
typedef std::map<std::string, EntityDefinition> EntityTypeMap;

class EntityDefinition {
public:
	EntityDefinition(){}
	~EntityDefinition(){}

	static void LoadEntityDefinitions( const XmlElement& EntityTypesElement );
	static EntityTypeMap s_definitions;

	explicit EntityDefinition( const XmlElement& entityElement );

public:
	float		m_radius = 0.1f;
	float		m_height = 1.f;
	float		m_walkSpeed = 1.f;
	std::string m_name = "";
};