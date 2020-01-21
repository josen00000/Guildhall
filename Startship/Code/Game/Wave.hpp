#pragma once
class Game;
class Wave {
public:
	Wave(){}
	~Wave(){}
	Wave(Game* inGame,int asteroidNum, int beetleNum,int waspNum);
public:
	void Startup();
	void Shutdown();
	void CreateEntities();


public:
	Game* m_game=nullptr;
	int m_asteroidNum=0;
	int m_beetleNum=0;
	int m_waspNum=0;
};