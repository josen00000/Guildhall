#include "Tile.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/MaterialDefinition.hpp"

Tile::Tile( IntVec2 tileCoords, RegionDefinition* regionDef )
	:m_tileCoords(tileCoords)
	,m_regDef(regionDef)
{
	m_box = AABB3( Vec3( (float)tileCoords.x, (float)tileCoords.y, 0 ), Vec3( (float)tileCoords.x + 1, (float)tileCoords.y + 1, 1 ) );
}

Tile::Tile( const Tile& copyFrom )
{
	m_tileCoords = copyFrom.m_tileCoords;
	m_regDef = copyFrom.m_regDef;
	m_box = copyFrom.m_box;
}

bool Tile::IsSolid() const
{
	return m_regDef->IsSolid();
}



void Tile::GetSideUVs( Vec2& uvAtMax, Vec2& uvAtMins ) const
{
	SpriteSheet* tileSheet = GetTileSpriteSheet();
	MaterialDefinition sideMaterial = m_regDef->m_materials["Side"];
	IntVec2 sideCoords = sideMaterial.m_spriteCoords;

	tileSheet->GetSpriteUVs( uvAtMins, uvAtMax, sideCoords );
}

void Tile::GetFloorUVs( Vec2& uvAtMax, Vec2& uvAtMins ) const
{
	SpriteSheet* tileSheet = GetTileSpriteSheet();
	MaterialDefinition floorMaterial = m_regDef->m_materials["Floor"];
	IntVec2 floorCoords = floorMaterial.m_spriteCoords;

	tileSheet->GetSpriteUVs( uvAtMins, uvAtMax, floorCoords );
}

void Tile::GetCeilingUVs( Vec2& uvAtMax, Vec2& uvAtMins ) const
{
	SpriteSheet* tileSheet = GetTileSpriteSheet();
	MaterialDefinition ceilingMaterial = m_regDef->m_materials["Ceiling"];
	IntVec2 ceilingCoords = ceilingMaterial.m_spriteCoords;

	tileSheet->GetSpriteUVs( uvAtMins, uvAtMax, ceilingCoords );
}

SpriteSheet* Tile::GetTileSpriteSheet() const
{
	SpriteSheet* materialSheet = MaterialDefinition::s_sheet.m_sheet;
	return materialSheet;
}

