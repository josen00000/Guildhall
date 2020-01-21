#pragma once
#include <string>
#include <map>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/XmlUtils.hpp"	
#include "Engine/Math/AABB2.hpp"

class ActorDefinition
{
public:
	ActorDefinition(){};
	~ActorDefinition(){};
	explicit ActorDefinition( const XmlElement& actorElement );
	static void PopulateDefinitionsFromXmlElement( XmlElement& actorDefElement );
private:
	void ParseFactionWithString( const std::string& faction );
	void ParseSizeElement( const XmlElement& element );
	void ParseHealthElement( const XmlElement& element );
	void ParseMovementElement( const XmlElement& element );
	void ParseAnimationSetElement( const XmlElement& element );
	void ParseAnimationElement( const XmlElement& element );

public:
	static std::map<std::string, ActorDefinition> s_definitions; // is pointer better
	std::string m_name = "";
	int m_faction = 0;
	float m_physicsRadius = 0.5f;
	AABB2 m_localDrawBound = AABB2( 0, 0, 1, 1 );
	float m_walkSpeed = 0.f;
	int m_maxHealth = 0;
	int m_startHealth = 0;
	std::map<std::string, std::vector<int>> m_animations;
	std::string m_spriteFilePath = "";
	IntVec2 m_spriteLayout = IntVec2( 0, 0 );
	int m_fps = 0;
};



