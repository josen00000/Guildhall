#include"Tile.hpp"
#include<Engine/Math/Vec3.hpp>
#include "Engine/Math/AABB2.hpp"
#include<Game/App.hpp>
#include<Engine/Renderer/RenderContext.hpp>
#include<Engine/core/EngineCommon.hpp>


extern RenderContext* g_theRenderer;


Tile::Tile( const Tile& copyFrom )
	:m_tileCoords(copyFrom.m_tileCoords)
	,m_type(copyFrom.m_type)
{	
}

Tile::Tile( IntVec2& tileCoords )
	:m_tileCoords(tileCoords)
{
	m_type=TILE_TYPE_GRASS;
}

Tile::Tile( int tileX, int tileY )
{
	m_tileCoords=IntVec2(tileX,tileY);
}

Tile::Tile( int tileX, int tileY, TileType tileType )
{
	m_tileCoords = IntVec2(tileX, tileY);
	m_type = tileType;
}

void Tile::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Tile::Render()const 
{
	Rgba8 tileColor;
	if(m_type==TILE_TYPE_GRASS){
		tileColor=Rgba8(0,100,0);
	}
	else{
		tileColor=Rgba8(211,211,211);
	}
	AABB2 temAABB2=GetBounds();
	g_theRenderer->DrawAABB2D(temAABB2,tileColor);
}

void Tile::DebugRender() const
{

}

AABB2 Tile::GetBounds() const
{
	Vec2 mins((float)m_tileCoords.x,(float)m_tileCoords.y);
	Vec2 maxs(mins.x+1.f, mins.y+1.f);
	return AABB2(mins, maxs);
}

void Tile::SetTileType( const TileType tileType )
{
	m_type=tileType;
}
