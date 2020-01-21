#pragma once
//#include<engine/Math/IntVec2.hpp>
class Player;
class Map;
class Turn  {
public:
	Turn(){}
	~Turn(){}
	Turn(Player* firstPlayer, Player* secondPlayer,Map* map);
public:
	void Startup();
	void Shutdown();
	void MovePlayer(Player* player);
	void SelectGrid(Player* player);
	void Update();

public:
	Player* m_player1=nullptr;
	Player* m_player2=nullptr;
	Map* m_map=nullptr;
	bool m_isMoving=true;
};