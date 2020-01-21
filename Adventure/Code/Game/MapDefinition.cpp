#include "MapDefinition.hpp"
#include "Game/App.hpp"
#include "Game/Worm.hpp"
#include "Game/Mutator.hpp"
#include "Game/FromImage.hpp"
#include "Game/PerlinNoise.hpp"
#include "Game/CellularAutomata.hpp"
#include "Game/MapGenStep.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

extern RenderContext* g_theRenderer;

std::map<std::string, MapDefinition> MapDefinition:: s_definitions;


MapDefinition::MapDefinition( XmlElement& mapDefElement )
{
	m_name							= ParseXmlAttribute( mapDefElement, "name", "UNNAMED" );
	m_width							= ParseXmlAttribute( mapDefElement, "width", 16 );
	m_height						= ParseXmlAttribute( mapDefElement, "height", 9 );
	m_fillTileType  				= ParseXmlAttribute( mapDefElement, "fillTile", "Grass" );
	m_edgeTileType  				= ParseXmlAttribute( mapDefElement, "edgeTile", "Water" );

	XmlElement* childElement = mapDefElement.FirstChildElement();
	XmlElement* mapGenStepElement = nullptr;
	while( childElement ){
		std::string tempName = childElement->Name();
		if(tempName.compare("MapGenerateSteps") == 0){
			mapGenStepElement = childElement;
			break;
		}
		childElement = childElement->NextSiblingElement();
	}
	
	ParseMapGenSteps( *mapGenStepElement );
	
}


const MapDefinition* MapDefinition::GetMapDefinitionByName( std::string mapName )
{
	auto temIterator = s_definitions.find(mapName);
	if(temIterator != s_definitions.end()){
		return &temIterator->second;
	}
	else{
		return nullptr;
	}
}

void MapDefinition::ParseMapGenSteps( XmlElement& mapGenElement )
{
	XmlElement* tempElement = mapGenElement.FirstChildElement(); 
	if( tempElement == nullptr ){
		
	}
	while( tempElement ){
		CreateMapGenStep( *tempElement );
		tempElement = tempElement->NextSiblingElement();
	}
}


void MapDefinition::CreateMapGenStep( XmlElement& mapGenStepElement )
{
	std::string stepName = mapGenStepElement.Name();
	MapGenStep* tempMapGenStep;
	if( stepName.compare( "Mutate" ) == 0 )				{ tempMapGenStep = new Mutator( mapGenStepElement ); }
	if( stepName.compare( "Worm" ) == 0 )				{ tempMapGenStep = new Worm( mapGenStepElement ); }
	if( stepName.compare( "FromImage" ) == 0 )			{ tempMapGenStep = new FromImage( mapGenStepElement ); }
	if( stepName.compare( "CellularAutomata" ) == 0 )	{ tempMapGenStep = new CellularAutomata( mapGenStepElement ); }
	if( stepName.compare( "PerlinNoise" ) == 0 )		{ tempMapGenStep = new PerlinNoise( mapGenStepElement ); }
//	if( stepName.compare( "Mutator" ) == 0 ) { tempMapGenStep = new Mutator( mapGenStepElement ); }

	m_mapGenSteps.push_back( tempMapGenStep );
}

void MapDefinition::PopulateDefinitionsFromXmlElement( XmlElement& tileDefElement )
{
	MapDefinition temTileDefinition = MapDefinition(tileDefElement);
	s_definitions[temTileDefinition.m_name] = temTileDefinition;
}

void MapDefinition::PopulateDefinitionsFromXmlFile( const char* filepath )
{
	XmlDocument dataFile;
	dataFile.LoadFile(filepath);
	if(!IfLoadXmlFileSucceed(dataFile)){ 
		// error handle 
		return; 
	}
	XmlElement* rootElement = dataFile.RootElement();
	XmlElement* temElement = rootElement->FirstChildElement();
	while( temElement ){
		PopulateDefinitionsFromXmlElement( *temElement );
		temElement = temElement->NextSiblingElement();
	}
}
