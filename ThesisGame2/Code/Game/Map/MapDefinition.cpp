#include "MapDefinition.hpp"
#include "Game/Map/CellularAutomata.hpp"
#include "Game/Map/MapGenStep.hpp"
#include "Game/Map/Mutator.hpp"
#include "Game/Map/GenRooms.hpp"
#include "Engine/Core/XmlUtils.hpp"

std::map<std::string, MapDefinition*> MapDefinition::s_definitions;

MapDefinition::MapDefinition( XmlElement& mapDefElement )
{
	m_name							= ParseXmlAttribute( mapDefElement, "name", "UNNAMED" );
	m_width							= ParseXmlAttribute( mapDefElement, "width", m_width );
	m_height						= ParseXmlAttribute( mapDefElement, "height", m_height );
	m_fillTileType  				= ParseXmlAttribute( mapDefElement, "fillTile", "Floor" );
	m_edgeTileType  				= ParseXmlAttribute( mapDefElement, "edgeTile", "Wall" );
	m_startTileType  				= ParseXmlAttribute( mapDefElement, "startTile", "Entry" );


	const XmlElement* mapGenStepsElement = mapDefElement.FirstChildElement();
	ParseMapGenSteps( *mapGenStepsElement );
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


void MapDefinition::ParseMapGenSteps( const XmlElement& mapGenStepsElement )
{
	const XmlElement* mapGenStepElement = mapGenStepsElement.FirstChildElement();
	while( mapGenStepElement ) {
		CreateMapGenStep( *mapGenStepElement );
		mapGenStepElement = mapGenStepElement->NextSiblingElement();
	}
}

void MapDefinition::CreateMapGenStep( const XmlElement& mapGenStepElement )
{
	std::string stepName = mapGenStepElement.Name();
	if( stepName == "Mutate" )			{ m_mapGenSteps.push_back( new Mutator( mapGenStepElement ) ); }
	if( stepName == "CellularAutomata" ){ m_mapGenSteps.push_back( new CellularAutomata( mapGenStepElement ) ); }
	if( stepName == "Room" )			{ m_mapGenSteps.push_back( new GenRooms( mapGenStepElement ) ); }
}
