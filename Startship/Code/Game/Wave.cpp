#include"Wave.hpp"
#include<Game/Game.hpp>
Wave::Wave( Game* inGame,int asteroidNum,int beetleNum,int waspNum )
	:m_game(inGame)
	,m_asteroidNum(asteroidNum)
	,m_beetleNum(beetleNum)
	,m_waspNum(waspNum)
{

}

void Wave::Startup()
{
	CreateEntities();
}

void Wave::Shutdown()
{

}

void Wave::CreateEntities()
{
  	for(int asteroidIndex=0;asteroidIndex<m_asteroidNum;asteroidIndex++){
		m_game->SpawnAsteroid();
	}
	for(int beetleIndex=0;beetleIndex<m_beetleNum;beetleIndex++){
		m_game->SpawnBeetle(*m_game->m_playerShip);
	}
	for(int waspIndex=0;waspIndex<m_waspNum;waspIndex++){
		m_game->SpawnWasp(*m_game->m_playerShip);
	}
}
