#pragma once
#include <map>
#include <string>
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

class ActorDefinition{
public:
	ActorDefinition(){}
	~ActorDefinition(){}
	explicit ActorDefinition( XmlElement const& actorElement );
public:
	static std::map<std::string, ActorDefinition> s_definitions;
	static void PopulateDefinitionFromXmlFile( std::string filePath );
public:
	std::string m_name = "";
	float m_walkSpeed = 0.f;
	float m_physicsRadius = 0.f;
	float m_health = 0.f;
	float m_attackStrength = 0.f;

	// sprite
	std::string m_spriteSheetPath = "";
	SpriteSheet* m_spriteSheet = nullptr;
	IntVec2 m_spriteLayout = IntVec2( 0 );
	std::map<std::string, SpriteAnimDefinition*> m_anims;
};