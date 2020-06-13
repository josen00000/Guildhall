#include "MapDefinition.hpp"

MapDefMap MapDefinition::s_definitions;

void MapDefinition::LoadMapDefinition( const XmlElement& mapElement )
{
	//MapDefinition mapDef = MapDefinition( mapElement );
	//s_definitions
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
	const XmlElement* playerStartElement = entitiesElement->FirstChildElement();
	m_playerStartPos = ParseXmlAttribute( *playerStartElement, "pos", m_playerStartPos );
	m_playerStartYaw = ParseXmlAttribute( *playerStartElement, "yaw", m_playerStartYaw );
}



