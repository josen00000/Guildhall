#pragma once
#include<engine/Math/vec2.hpp>
#include<engine/Math/IntVec2.hpp>
#include<vector>
#include<Engine/Core/Vertex_PCU.hpp>
#include<Engine/Core/Rgba8.hpp>
#include<Game/GameCommon.hpp>
class Game;
class Map;
class Tile {
public:
	Tile()=default;
	~Tile()=default;
	Tile(const Tile& copyFrom);
	//explicit
	explicit Tile(Game* iniGame,Map* iniMap, Vec2& iniPosInWorld, IntVec2& iniPosInTileCoords);
	void Createvertices();
	void Render();
	void Update();
	
	void Startup();
	Vec3 GetRenderPosition(Vec3 localPos);
	void RenderBase();
	void RenderSides();
	void Spread();
	bool IsSpreadComplete();
private:
	Vec2 GetWorldPosForTileCoords();


public:
	Game* m_game=nullptr;
	Map* m_map=nullptr;
	bool m_isTriangleGetSpread[4]={false,false,false,false};// down,right,left,up
	Vec2 m_centerPosInWorld=Vec2(0.f,0.f);
	IntVec2 m_posInTileCoords=IntVec2(0,0);
	Vertex_PCU m_vertices[TILE_VERTEX_NUM]={};
	float m_sideThick=TILE_SIDE_THICKNESS;
	int m_spreadNum=0;
	bool m_isSelected=false;
	bool m_isSelecting=false;
	bool m_isSpreading=false;
	Rgba8 m_selectingColor;
	Rgba8 m_spreadingColor;
	Rgba8 m_baseColor;
	Rgba8 m_gridColor;
	Rgba8 m_sideColor;

};