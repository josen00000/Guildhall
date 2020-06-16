#pragma once
#include <map>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/IntVec2.hpp"

class TileDefinition;

typedef std::map<std::string, TileDefinition*> TileDefMap; 

class TileDefinition {
public:
	explicit TileDefinition( const XmlElement& tileDefElement );

	static void PopulateDefinitionFromXmlFile( const char* filePath );

public:
	// Accessor
	bool		DoesAllowWalk() const { return m_allowsWalk; }
	std::string GetName() const { return m_name; }
	Rgba8		GetTintColor() const { return m_tintColor; }
	Vec2		GetSpriteUVAtMins() const { return m_uvAtMins; }
	Vec2		GetSpriteUVAtMaxs() const { return m_uvAtMaxs; }
	IntVec2		GetSpriteCoords() const { return m_spriteCoords; }

public:
	static TileDefMap	s_definitions;
	static std::string	s_spriteFilePath;
	static IntVec2		s_spriteLayout;

	bool				m_allowsWalk = true;
	std::string			m_name = "";
	IntVec2				m_spriteCoords = IntVec2();
	Rgba8				m_tintColor = Rgba8::WHITE;

	// sprite sheet uv
	Vec2 m_uvAtMins = Vec2::ZERO;
	Vec2 m_uvAtMaxs = Vec2::ONE;
};
