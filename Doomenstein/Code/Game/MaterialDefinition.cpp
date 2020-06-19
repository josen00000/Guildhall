#include "MaterialDefinition.hpp"
#include "Engine/Core/DevConsole.hpp"

extern DevConsole* g_theConsole;

static void LoadMaterialsSheet( const XmlElement& sheetElement );
static void LoadMaterialTypes( const XmlElement& materialElement );

MaterialTypeMap MaterialDefinition::s_definitions;
MaterialsSheet MaterialDefinition::s_sheet;
MaterialDefinition MaterialDefinition::s_defaultMaterial;

// static method
void LoadMaterialsSheet( const XmlElement& sheetElement )
{
	MaterialDefinition::s_sheet = MaterialsSheet( sheetElement );
}

void LoadMaterialTypes( const XmlElement& materialElement )
{
	MaterialDefinition materialDef = MaterialDefinition( materialElement );
	MaterialDefinition::s_definitions[materialDef.m_name] = materialDef;
}

void MaterialDefinition::LoadMapMaterialDefinitions( const XmlElement& mapMaterialElement )
{
	std::string elementName = mapMaterialElement.Name();
	if( elementName.compare( "MapMaterialTypes" ) != 0 ) {
		g_theConsole->DebugErrorf( "Root Name Error. %s should be %s", elementName.c_str(), "MapMaterialTypes" );
	}

	std::string defaultTypeName = ParseXmlAttribute( mapMaterialElement, "default", "" );
	const XmlElement* sheetElement = mapMaterialElement.FirstChildElement();
	LoadMaterialsSheet( *sheetElement );
	const XmlElement* materialElement = sheetElement->NextSiblingElement();
	while( materialElement ) {
		LoadMaterialTypes( *materialElement );
		materialElement = materialElement->NextSiblingElement();
	}
	s_defaultMaterial = s_definitions[defaultTypeName];
}

MaterialDefinition::MaterialDefinition( const XmlElement& materialElement )
{
	m_name			= ParseXmlAttribute( materialElement, "name", "" );
	m_sheetName		= ParseXmlAttribute( materialElement, "sheet", "" );
	m_spriteCoords	= ParseXmlAttribute( materialElement, "spriteCoords", IntVec2::ZERO );
	if( !isSpriteCoordsValid( m_spriteCoords ) ) {
		g_theConsole->DebugErrorf( "material \"%s\" sprite coords invalid!", m_name.c_str() );
		*this = s_defaultMaterial;
	}

}

bool MaterialDefinition::isSpriteCoordsValid( IntVec2 spriteCoords )
{
	return ( spriteCoords.x >= 0 && spriteCoords.x < s_sheet.m_layout.x && spriteCoords.y >= 0 && spriteCoords.y < s_sheet.m_layout.y );
}

