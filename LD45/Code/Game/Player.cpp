#include"Player.hpp"
#include<Game/Map.hpp>
#include<engine/Core/Rgba8.hpp>
#include<game/Game.hpp>
Player::Player( Game* inGame, bool inIsBlack,Map* inMap )
	:m_game(inGame)
	,m_isBlack(inIsBlack)
	,m_map(inMap)
{

}

void Player::Startup()
{
	if(m_isBlack){
		m_color=Rgba8(255,255,255);
	}
	else{
		m_color=Rgba8(0,0,0);
	}

}



void Player::Move( bool isUp, bool isDown, bool isRight, bool isLeft )
{
	IntVec2 mapSize=m_map->GetMapSize();
	m_lastTilePos=m_tilePos;
	if( isUp ) {
		m_tilePos.y++;
	}
	if(isDown) {
		m_tilePos.y--;
	}
	if(isLeft){
		m_tilePos.x--;
	}
	if(isRight){
		m_tilePos.x++;
	}
	
	
	if(!m_map->IsValidTileCoords(m_tilePos)){
		m_tilePos=m_lastTilePos;
	}
	
}

void Player::InitialEachTurn()
{
	m_tilePos=IntVec2(INI_PLAYER_POS_X,INI_PLAYER_POS_Y);
}

const void Player::Render()
{
	Vec2 temVec2(m_tilePos.x+1,m_tilePos.y+1);
	//m_game->m_theRenderer->DrawLine(Vec2(0,0),temVec2,1,Rgba8(255,0,0));
}

void Player::SelectTile(Tile& inTile)
{
	m_selectedTile.push_back(inTile);
}

void Player::SpreadGrids()
{
	for( int tileIndex=0; tileIndex<m_selectedTile.size(); tileIndex++ ) {
		m_map->SpreadGrid(tileIndex);
	
	}
}

