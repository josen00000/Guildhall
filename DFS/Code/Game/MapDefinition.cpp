#include "MapDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"

std::map<std::string, MapDefinition*> MapDefinition::s_definitions;

MapDefinition::MapDefinition( XmlElement& mapDefElement )
{
	m_name							= ParseXmlAttribute( mapDefElement, "name", "UNNAMED" );
	m_width							= ParseXmlAttribute( mapDefElement, "width", 11 );
	m_height						= ParseXmlAttribute( mapDefElement, "height", 11 );
	m_fillTileType  				= ParseXmlAttribute( mapDefElement, "fillTile", "Floor" );
	m_edgeTileType  				= ParseXmlAttribute( mapDefElement, "edgeTile", "Wall" );
}

const MapDefinition* MapDefinition::GetMapDefByName( std::string mapName )
{
	auto mapIter = s_definitions.find( mapName );
	if( mapIter != s_definitions.end() ) {
		return mapIter->second;
	}
	return nullptr;
}

void MapDefinition::PopulateDefinitionFromXmlElement( XmlElement& tileDefElement )
{
	MapDefinition* temTileDef = new MapDefinition( tileDefElement );
	s_definitions[temTileDef->GetName()] = temTileDef;
}

void MapDefinition::PopulateDefinitionsFromXmlFile( const char* filepath )
{
	XmlDocument defFile;
	defFile.LoadFile( filepath );
	if( !IfLoadXmlFileSucceed( defFile ) ) {
		ERROR_RECOVERABLE( Stringf( "file path: %s wrong when loading map def file.", filepath ));
		return;
	}

	XmlElement* rootElement = defFile.RootElement();
	XmlElement* temElement = rootElement->FirstChildElement();
	while( temElement ) {
		PopulateDefinitionFromXmlElement( *temElement );
		temElement = temElement->NextSiblingElement();
	}
}
