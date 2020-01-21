#include "TileDefinition.hpp"
#include "Game/App.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

extern RenderContext* g_theRenderer;
std::map<std::string, TileDefinition*> TileDefinition:: s_definitions;
std::string TileDefinition::s_spriteFilePath;
IntVec2 TileDefinition::s_spriteLayout;

TileDefinition::TileDefinition( const XmlElement& tileDefElement )
{
	m_name				= ParseXmlAttribute( tileDefElement, "name", "UNNAMED");
	m_spriteCoords		= ParseXmlAttribute( tileDefElement, "spriteCoords", IntVec2(0, 0));
	m_tintColor			= ParseXmlAttribute( tileDefElement, "spriteTint", Rgba8::WHITE);
	m_fromImageColor	= ParseXmlAttribute( tileDefElement, "fromImageColor", Rgba8::WHITE );
	m_allowsSight		= ParseXmlAttribute( tileDefElement, "allowsSight", false );
	m_allowsWalk 		= ParseXmlAttribute( tileDefElement, "allowsWalk", false );
	m_allowsSwim 		= ParseXmlAttribute( tileDefElement, "allowsSwim", false );
	m_allowsFly 		= ParseXmlAttribute( tileDefElement, "allowsFly", false );
	if(s_spriteFilePath == ""){
		ERROR_AND_DIE( "Try to load sprite file with empty path in TileDefinition.cpp!");
	}
	Texture* tileTexture = g_theRenderer->CreateOrGetTextureFromFile( s_spriteFilePath.c_str() );
 	int spriteIndex = m_spriteCoords.x + m_spriteCoords.y * s_spriteLayout.x;
	SpriteSheet spriteSheet = SpriteSheet( *tileTexture, s_spriteLayout );
	spriteSheet.GetSpriteUVs( m_uvAtMaxs, m_uvAtMins, spriteIndex );
}


void TileDefinition::GetSpriteUVs( Vec2& uvAtMins, Vec2& uvAtMaxs ) const
{
	uvAtMins = m_uvAtMins;
	uvAtMaxs = m_uvAtMaxs;
}

bool TileDefinition::GetTileAllowsSight() const
{
	return m_allowsSight;
}

bool TileDefinition::GetTileAllowsWalk() const
{
	return m_allowsWalk;
}

bool TileDefinition::GetTileAllowsSwim() const
{
	return m_allowsSwim;
}

bool TileDefinition::GetTileAllowsFly() const
{
	return m_allowsFly;
}

Rgba8 TileDefinition::GetTintColor() const
{
	return m_tintColor;
}

void TileDefinition::ClearDefinitions()
{
	s_definitions.clear();
}

void TileDefinition::PopulateDefinitionsFromXmlElement( const XmlElement& tileDefElement )
{
	TileDefinition* temTileDefinition = new TileDefinition(tileDefElement);
	std::pair<std::string, TileDefinition*> tempPair( temTileDefinition->m_name, temTileDefinition );
	s_definitions.insert( tempPair );
}

void TileDefinition::ParseSpriteSheetFromXmlElement( const XmlElement& spriteElement )
{
	s_spriteFilePath = ParseXmlAttribute( spriteElement, "spriteSheet", "");
	s_spriteLayout	 = ParseXmlAttribute( spriteElement, "spriteLayout", IntVec2( 0, 0 ) );

}

