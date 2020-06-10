#include "TileDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

extern RenderContext* g_theRenderer;

TileDefMap TileDefinition::s_definitions;
std::string TileDefinition::s_spriteFilePath;
IntVec2 TileDefinition::s_spriteLayout;

TileDefinition::TileDefinition( const XmlElement& tileDefElement )
{
	m_name				= ParseXmlAttribute( tileDefElement, "name", "UNNAMED" );
	m_spriteCoords		= ParseXmlAttribute( tileDefElement, "spriteCoords", IntVec2( 0, 0 ) );
	m_tintColor			= ParseXmlAttribute( tileDefElement, "spriteTint", Rgba8::WHITE );
	m_allowsWalk 		= ParseXmlAttribute( tileDefElement, "allowsWalk", true );	
	if( s_spriteFilePath == "" ) {
		ERROR_AND_DIE( "Try to load sprite file with empty path in tiledefinition!" );
	}
	Texture* tileTexture = g_theRenderer->CreateOrGetTextureFromFile( s_spriteFilePath.c_str() );
	SpriteSheet spriteSheet = SpriteSheet( tileTexture, s_spriteLayout ); 
	//int spriteSheet.GetSpriteUVS( m_uvatma)
}

