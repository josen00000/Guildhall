#include "EntityDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"

EntityTypeMap EntityDefinition::s_definitions;

void EntityDefinition::LoadEntityDefinitions( const XmlElement& EntityTypesElement )
{
	const XmlElement* entityTypeElement = EntityTypesElement.FirstChildElement();

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
