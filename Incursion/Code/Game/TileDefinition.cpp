#include "TileDefinition.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

extern RenderContext* g_theRenderer;

std::vector<TileDefinition> TileDefinition:: s_definitions;
TileDefinition::TileDefinition( Rgba8 tintColor, bool isSolid, const Vec2 uvAtMins, const Vec2 uvAtMaxs )
	:m_tintColor(tintColor)
	,m_isSolid(isSolid)
	,m_uvAtMins(uvAtMins)
	,m_uvAtMaxs(uvAtMaxs)
{

}

/*
void TileDefinition::AddTileDefinition(  Rgba8 tintColor, bool isSolid, const Vec2 uvAtMins, const Vec2 uvAtMaxs )
{
	s_definitions.push_back( TileDefinition(tintColor, isSolid, uvAtMins, uvAtMaxs));
}*/



bool TileDefinition::GetTileIsSolid(  ) const
{
	return m_isSolid;
}


void TileDefinition::GetSpriteUVs( Vec2& uvAtMins, Vec2& uvAtMaxs ) const
{
	uvAtMins = m_uvAtMins;
	uvAtMaxs = m_uvAtMaxs;
}

Rgba8 TileDefinition::GetTintColor() const
{
	return m_tintColor;
}

void TileDefinition::ClearDefinitions()
{
	s_definitions.clear();
}

void TileDefinition::InitializeDefinitions()
{
	Texture* tileTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	SpriteSheet* m_spriteSheet = new SpriteSheet( *tileTexture, IntVec2( 8, 8 ) );
	Vec2 uvAtMins_grass = Vec2(0,0);
	Vec2 uvAtMaxs_grass = Vec2(0,0);
	Vec2 uvAtMins_rock = Vec2(0,0);
	Vec2 uvAtMaxs_rock = Vec2(0,0);
	Vec2 uvAtMins_mud = Vec2( 0, 0 );
	Vec2 uvAtMaxs_mud = Vec2( 0, 0 );
	Vec2 uvAtMins_brick = Vec2( 0, 0 );
	Vec2 uvAtMaxs_brick = Vec2( 0, 0 );
	Vec2 uvAtMins_lava = Vec2( 0, 0 );
	Vec2 uvAtMaxs_lava = Vec2( 0, 0 );
	Vec2 uvAtMins_water = Vec2( 0, 0 );
	Vec2 uvAtMaxs_water = Vec2( 0, 0 );

	const SpriteDefinition& grassSpDef = m_spriteSheet->GetSpriteDefinition( 1 );
	grassSpDef.GetUVs( uvAtMins_grass, uvAtMaxs_grass );
	const SpriteDefinition& rockSpDef = m_spriteSheet->GetSpriteDefinition( 26 );
	rockSpDef.GetUVs( uvAtMins_rock, uvAtMaxs_rock );
	const SpriteDefinition& mudSpDef = m_spriteSheet->GetSpriteDefinition( 20 );
	mudSpDef.GetUVs( uvAtMins_mud, uvAtMaxs_mud );
	const SpriteDefinition& brickSpDef = m_spriteSheet->GetSpriteDefinition( 31 );
	brickSpDef.GetUVs( uvAtMins_brick, uvAtMaxs_brick );
	const SpriteDefinition& lavaSpDef = m_spriteSheet->GetSpriteDefinition( 62 );
	lavaSpDef.GetUVs( uvAtMins_lava, uvAtMaxs_lava );
	const SpriteDefinition& waterSpDef = m_spriteSheet->GetSpriteDefinition( 61 );
	waterSpDef.GetUVs( uvAtMins_water, uvAtMaxs_water );

	s_definitions.push_back(TileDefinition(Rgba8(0,0,0,0), false, uvAtMins_grass, uvAtMaxs_grass));
	s_definitions.push_back(TileDefinition(Rgba8(0,0,0,0), true, uvAtMins_rock, uvAtMaxs_rock));
	s_definitions.push_back(TileDefinition(Rgba8(0,0,0,0), false, uvAtMins_mud, uvAtMaxs_mud));
	s_definitions.push_back(TileDefinition(Rgba8(0,0,0,0), true, uvAtMins_brick, uvAtMaxs_brick));
	s_definitions.push_back(TileDefinition(Rgba8(0,0,0,0), false, uvAtMins_lava, uvAtMaxs_lava));
	s_definitions.push_back(TileDefinition(Rgba8(0,0,0,0), true, uvAtMins_water, uvAtMaxs_water));
}
