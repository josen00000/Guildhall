#include"Turn.hpp"
#include<Game/Player.hpp>
#include<Game/Map.hpp>

Turn::Turn( Player* firstPlayer, Player* secondPlayer, Map* map )
	:m_player1(firstPlayer)
	,m_player2(secondPlayer)
	,m_map(map)
{

}

void Turn::Startup()
{

}

void Turn::Shutdown()
{

}

void Turn::MovePlayer( Player* player )
{

}

void Turn::SelectGrid( Player* player )
{

}

void Turn::Update()
{
	
}

