#include "ActorDefinition.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_theRenderer;

std::map<std::string, ActorDefinition> ActorDefinition::s_definitions;

ActorDefinition::ActorDefinition( XmlElement const& actorElement )
{
	if( strcmp( actorElement.Name(), "ActorDefinition" ) != 0 ){ 
		ERROR_AND_DIE( "wrong element name " );
	}
	const XmlElement* sizeElement	= actorElement.FirstChildElement();
	const XmlElement* moveElement	= sizeElement->NextSiblingElement();
	const XmlElement* healthElement = moveElement->NextSiblingElement();
	const XmlElement* fightElement	= healthElement->NextSiblingElement();
	const XmlElement* spriteElement = fightElement->NextSiblingElement();
	

	m_name				= ParseXmlAttribute( actorElement, "name", "" );
	m_physicsRadius		= ParseXmlAttribute( *sizeElement, "physicsRadius", 0.f );
	m_walkSpeed			= ParseXmlAttribute( *moveElement, "speed", 0.f );
	m_health			= ParseXmlAttribute( *healthElement, "max", 0.f );
	m_attackStrength	= ParseXmlAttribute( *fightElement, "attackStrength", 0.f );

	m_spriteSheetPath	= ParseXmlAttribute( *spriteElement, "spriteSheet", "" );
	m_spriteLayout		= ParseXmlAttribute( *spriteElement, "spriteLayout", m_spriteLayout );
	Texture* spriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( m_spriteSheetPath.c_str() );
	m_spriteSheet = new SpriteSheet( spriteSheetTexture, m_spriteLayout );

	const XmlElement* spriteAnimElement = spriteElement->FirstChildElement();
	while( spriteAnimElement ) {
		std::string spriteAnimName	= ParseXmlAttribute( *spriteAnimElement, "name", "" );
		Strings spriteAnimIndexesInString	= ParseXmlAttribute( *spriteAnimElement, "spriteIndexes", Strings(), "," );
		int spriteAnimNum = (int)spriteAnimIndexesInString.size();
		int* spriteAnimIndexes = new int[spriteAnimNum];

		for( int i = 0; i < spriteAnimNum; i++ ) {
			spriteAnimIndexes[i] = GetIntFromText( spriteAnimIndexesInString[i].c_str() );
		}
		SpriteAnimDefinition* tempAnimDef = new SpriteAnimDefinition( *m_spriteSheet, spriteAnimIndexes, spriteAnimNum, 0.1f );
		m_anims[spriteAnimName] = tempAnimDef;

		spriteAnimElement = spriteAnimElement->NextSiblingElement();
	}

}

void ActorDefinition::PopulateDefinitionFromXmlFile( std::string filePath )
{
	XmlDocument actorFile;
	actorFile.LoadFile( filePath.c_str() );
	
	XmlElement* fileElement = actorFile.FirstChildElement();

	const XmlElement* actorElement = fileElement->FirstChildElement();
	while ( actorElement )
	{
		ActorDefinition actorDef = ActorDefinition( *actorElement );
		s_definitions[actorDef.m_name] = actorDef;
		actorElement = actorElement->NextSiblingElement();
	}
}
