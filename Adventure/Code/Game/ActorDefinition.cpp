#include "ActorDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"

std::map<std::string, ActorDefinition> ActorDefinition::s_definitions;

ActorDefinition::ActorDefinition( const XmlElement& actorElement )
{
	m_animations.empty();
	m_name = ParseXmlAttribute( actorElement, "name", "");
	std::string faction = ParseXmlAttribute( actorElement, "faction", "neutral" );
	ParseFactionWithString( faction	);	
	const XmlElement* sizeElement		= FindXmlChildElementWithName( actorElement, "Size" );
	const XmlElement* movementElement	= FindXmlChildElementWithName( actorElement, "Movement" );
	const XmlElement* healthElement		= FindXmlChildElementWithName( actorElement, "Health" );
	const XmlElement* spriteAnimElement	= FindXmlChildElementWithName( actorElement, "SpriteAnimSet" );

	ParseSizeElement( *sizeElement );
	ParseMovementElement( *movementElement );
	ParseHealthElement( *healthElement );
	ParseAnimationSetElement( *spriteAnimElement );
}

void ActorDefinition::PopulateDefinitionsFromXmlElement( XmlElement& actorDefElement )
{
	ActorDefinition tempDef = ActorDefinition( actorDefElement );
	std::pair<std::string, ActorDefinition> tempPair( tempDef.m_name, tempDef );
	s_definitions.insert( tempPair );
}

void ActorDefinition::ParseFactionWithString( const std::string& faction )
{
	if( faction.compare( "good" ) == 0 ){
		m_faction = 1;
	}
	else if( faction.compare( "neutral" ) == 0 ){
		m_faction = 0;
	}
	else /* faction.compare( "evil" ) == 0 */{
		m_faction = -1;
	}
}

void ActorDefinition::ParseSizeElement( const XmlElement& element )
{
	m_physicsRadius		= ParseXmlAttribute( element, "physicsRadius", 0.5f );
	Strings defaultValue = {"-0.5", "-0.3", "0.5", "1.033" };
	Strings localDrawBounds = ParseXmlAttribute( element, "localDrawBounds", defaultValue, "," );
	
	float boundsMinX =  GetFloatFromText( localDrawBounds[0].c_str() );
	float boundsMinY =  GetFloatFromText( localDrawBounds[1].c_str() );
	float boundsMaxX =  GetFloatFromText( localDrawBounds[2].c_str() );
	float boundsMaxY =  GetFloatFromText( localDrawBounds[3].c_str() );
	m_localDrawBound = AABB2( boundsMinX, boundsMinY, boundsMaxX, boundsMaxY );
}

void ActorDefinition::ParseHealthElement( const XmlElement& element )
{
	m_maxHealth		= ParseXmlAttribute( element, "max", 100 );
	m_startHealth	= ParseXmlAttribute( element, "start", 100 );
}

void ActorDefinition::ParseMovementElement( const XmlElement& element )
{
	const XmlElement* walkElement = FindXmlChildElementWithName( element, "Walk" );
	m_walkSpeed = ParseXmlAttribute( *walkElement, "speed", 2.f );
}

void ActorDefinition::ParseAnimationSetElement( const XmlElement& element )
{
	m_spriteFilePath	= ParseXmlAttribute( element, "spriteSheet", "");
	m_spriteLayout		= ParseXmlAttribute( element, "spriteLayout", IntVec2( 0, 0 ) );
	m_fps				= ParseXmlAttribute( element, "fps", 10 );

	const XmlElement* animationElement = element.FirstChildElement();
	while( animationElement ){
		ParseAnimationElement( *animationElement );
		animationElement = animationElement->NextSiblingElement();
	}
}

void ActorDefinition::ParseAnimationElement( const XmlElement& element )
{
	std::string animName = ParseXmlAttribute( element, "name", "");
	Strings animIndexes = ParseXmlAttribute( element, "spriteIndexes", Strings(), ",");
	std::vector<int> indexes;
	for(int i = 0; i < animIndexes.size(); ++i ){
		int tempIndex = GetIntFromText( animIndexes[i].c_str() );
		indexes.push_back( tempIndex );
	}
	m_animations.insert( std::pair<std::string, std::vector<int>>( animName, indexes ));
}
