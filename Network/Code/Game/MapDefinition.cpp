#include "MapDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

MapDefMap MapDefinition::s_definitions;

void MapDefinition::LoadMapDefinition( const XmlElement& mapElement )
{
	//MapDefinition mapDef = MapDefinition( mapElement );
	//s_definitions
	UNUSED(mapElement);
}

MapDefinition::MapDefinition( const XmlElement& mapElement )
{
	m_type			= ParseXmlAttribute( mapElement, "type", m_type );
	m_version		= ParseXmlAttribute( mapElement, "version", m_version );
	m_dimensions	= ParseXmlAttribute( mapElement, "dimensions", m_dimensions );

	const XmlElement* legendElement = mapElement.FirstChildElement();
	const XmlElement* tileElement = legendElement->FirstChildElement();
	while( tileElement ) {
		std::string glyph		= ParseXmlAttribute( *tileElement, "glyph", "" );
		std::string regionType	= ParseXmlAttribute( *tileElement, "regionType", "" );
		m_lengends[glyph] = regionType;
		tileElement = tileElement->NextSiblingElement();
	}

	const XmlElement* mapRowsElement = legendElement->NextSiblingElement();
	const XmlElement* mapRowElement = mapRowsElement->FirstChildElement();
	while( mapRowElement ) {
		std::string row = ParseXmlAttribute( *mapRowElement, "tiles", "" );
		m_mapRows.push_back( row );
		mapRowElement = mapRowElement->NextSiblingElement();
	}

	const XmlElement* entitiesElement = mapRowsElement->NextSiblingElement();
	const XmlElement* entityElement = entitiesElement->FirstChildElement();
	while( entityElement ) {
		if( std::strcmp( entityElement->Name(), "PlayerStart" ) == 0 ) {
			m_playerStartPos = ParseXmlAttribute( *entityElement, "pos", m_playerStartPos );
			m_playerStartYaw = ParseXmlAttribute( *entityElement, "yaw", m_playerStartYaw );
		}
		else if( std::strcmp( entityElement->Name(), "Teleporter" ) == 0 ){
			Vec2 porterPos = ParseXmlAttribute( *entityElement, "pos", Vec2::ZERO );
			float porterYaw = ParseXmlAttribute( *entityElement, "yaw", 0.f );
			Vec2 targetPos = ParseXmlAttribute( *entityElement, "targetPos", Vec2::ZERO );
			std::string targetMap = ParseXmlAttribute( *entityElement, "targetMap", "" );
			TeleportInfo tempInfo = TeleportInfo{ porterYaw, porterPos, targetPos, targetMap };
			m_teleInfos.push_back( tempInfo );

		}
	}
}



