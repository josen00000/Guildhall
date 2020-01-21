#pragma once
#include<Game/Entity.hpp>
#include<Game/GameCommon.hpp>
#include<engine/Math/IntVec2.hpp>
#include<vector>	
class Game;
class Map;
class Tile;
class Player : Entity {
public:
	Player(){}
	~Player(){}
	Player(Game* inGame, bool inIsBlack,Map* inMap);


public:
	void Startup();
	void Move(bool isUp, bool isDown,bool isRight, bool isLeft);
	void InitialEachTurn();
	const void Render();
	void SelectTile(Tile& inTile);
	void SpreadGrids();
private:
public:
	Game* m_game=nullptr;
	Map* m_map=nullptr;
	std::vector<Tile> m_selectedTile;
	std::vector<Tile> m_spreadingTile;
	IntVec2	m_tilePos=IntVec2(INI_PLAYER_POS_X,INI_PLAYER_POS_Y);
	IntVec2 m_lastTilePos=IntVec2(-1,-1);
	Rgba8 m_color;
	bool m_isBlack=false;
};