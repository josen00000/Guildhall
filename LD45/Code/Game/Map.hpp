#pragma once
#include<Engine/Math/IntVec2.hpp>
#include<vector>
#include<Engine//Math/vec2.hpp>
#include<Game/Tile.hpp>
#include<Game/GameCommon.hpp>
struct IntVec2;
class Game;
class Player;
struct Rgba8;
class Map {
public:
	Map(){}
	~Map(){}
	explicit Map(Game* iniGame,IntVec2 iniSize,Vec2 iniPos);
	int GetTileIndexForTileCoords(const IntVec2& tileCoords) const;
	IntVec2 GetTileCoordsForTileIndex(const int tileIndex) const;
	IntVec2 GetMapSize() const {return m_size;}
	void SetPlayerPos(Player& inPlayer);
	void Update();
	bool IsValidTileCoords(IntVec2 inTileCoords);
	bool IsValidTileIndex(int inIndex);
	//void ResetTiles();
	void UpdateTiles();
	void RenderTiles();
	void Startup();
	void SelectGridByPlayer(Player& inPlayer);
	void SpreadGrids();
	void SpreadGrid(int inTileIndex);
	void SpreadToNeighbour(int inIndex,int triangleIndex,Rgba8 inSpreadColor);
	bool CheckSuccess();
private:
	void CreateTiles();
	void SetTileSelectingColor(int tileIndex);
public:
	Rgba8 m_baseColor=Rgba8(128,128,128);
	Rgba8 m_selectingColor;
	Rgba8 m_sideColor=Rgba8(50,50,50);

private:
	Game* m_game=nullptr;
	//Player* player1=nullptr;
	//Player* player2=nullptr;
	
	IntVec2 m_size=IntVec2();
	int m_sideLength=NORMAL_TILE_SIDE_LENGTH;
	Vec2 m_pos=Vec2();
	int m_tileNum=0;
	std::vector<Tile> m_tiles;
};