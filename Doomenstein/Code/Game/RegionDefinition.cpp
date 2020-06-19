#include "RegionDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

RegionTypeMap RegionDefinition::s_definitions;
RegionDefinition RegionDefinition::s_defaultRegion;

static void LoadRegion( const XmlElement& regionElement );

void LoadRegion( const XmlElement& regionElement )
{
	RegionDefinition* RegionDef = new RegionDefinition( regionElement );
	RegionDefinition::s_definitions[RegionDef->m_name] = RegionDef;
}

void RegionDefinition::LoadMapRegionDefinitions( const XmlElement& mapRegionElement )
{
	std::string elementName = mapRegionElement.Name();
	if( elementName.compare( "MapRegionTypes" ) != 0 ) {
		g_theConsole->DebugErrorf( "Root Name Error. %s should be %s", elementName.c_str(), "MapRegionTypes" );
	}
	const XmlElement* regionElement = mapRegionElement.FirstChildElement();
	std::string defaultRegionType = ParseXmlAttribute( mapRegionElement, "default", "" );

	while( regionElement ) {
		LoadRegion( *regionElement );
		regionElement = regionElement->NextSiblingElement();
	}
	s_defaultRegion = *s_definitions[defaultRegionType];
}

bool RegionDefinition::IsRegionTypeValid( std::string regionType )
{
	auto iter = s_definitions.find( regionType );
	return ( iter != s_definitions.end());
}

RegionDefinition::RegionDefinition( const XmlElement& regionElement )
{
	m_name		= ParseXmlAttribute( regionElement, "name", m_name );
	m_isSolod	= ParseXmlAttribute( regionElement, "isSolid", m_isSolod );

	const XmlElement* materialElement = regionElement.FirstChildElement();
	
	while( materialElement ) {
		std::string regionType = materialElement->Name();
		std::string materialName = ParseXmlAttribute( *materialElement, "material", "" );
		if( MaterialDefinition::s_definitions.find( materialName ) == MaterialDefinition::s_definitions.end() ) {
			m_materials[regionType] = MaterialDefinition::s_defaultMaterial;
			g_theConsole->DebugErrorf( " Try to load region \"%s\" with invalid material type \"%s\"!", regionType.c_str(), materialName.c_str() );
			//ERROR_AND_DIE( "Try to load region with invalid material type! " );
		}
		else {
			m_materials[regionType] = MaterialDefinition::s_definitions[materialName];
		}
		materialElement = materialElement->NextSiblingElement();
	}
}
