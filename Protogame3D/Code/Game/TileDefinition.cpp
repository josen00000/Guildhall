#include "TileDefinition.hpp"





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
}