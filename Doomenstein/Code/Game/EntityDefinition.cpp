#include "EntityDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/DevConsole.hpp"

extern DevConsole* g_theConsole;

EntityTypeMap EntityDefinition::s_definitions;

void EntityDefinition::LoadEntityDefinitions( const XmlElement& entityTypesElement )
{
	const XmlElement* entityTypeElement = entityTypesElement.FirstChildElement();
	std::string elementName = entityTypesElement.Name();
	if( elementName.compare( "EntityTypes" ) != 0 ) {
		g_theConsole->DebugErrorf( "Root Name Error. %s should be %s", elementName.c_str(), "EntityTypes" );
	}


	while( entityTypeElement ) {
		EntityDefinition entityDef = EntityDefinition( *entityTypeElement );
		s_definitions[entityDef.m_name] = entityDef;
		entityTypeElement = entityTypeElement->NextSiblingElement();
	}
}

EntityDefinition::EntityDefinition( const XmlElement& entityElement )
{
	m_name		= ParseXmlAttribute( entityElement, "name", m_name ); 
	const XmlElement* physicsElement = entityElement.FirstChildElement();
	m_radius	= ParseXmlAttribute( *physicsElement, "radius", m_radius );
	m_height	= ParseXmlAttribute( *physicsElement, "height", m_height );
	m_walkSpeed	= ParseXmlAttribute( *physicsElement, "walkSpeed", m_walkSpeed );

}
