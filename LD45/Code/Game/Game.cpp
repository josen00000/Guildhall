#include "Game.hpp"
#include <windows.h>
#include <Engine/Core/EngineCommon.hpp>
#include<Game/Map.hpp>
#include<Game/Player.hpp>
#include<Game/Turn.hpp>


const XboxController* playerController=nullptr;



Game::Game( RenderContext* gameRenderer , InputSystem* inputSystem,bool isDevelop,Camera* inCamera)
	:m_theRenderer(gameRenderer)
	,m_theInput(inputSystem)
	,m_developMode(isDevelop)
	,m_camera(inCamera)
{
	m_rng=RandomNumberGenerator();
	playerController=&m_theInput->GetXboxController(0);
}

void Game::Startup()
{
	m_map=new Map(this,IntVec2(MAP_SIZE_X,MAP_SIZE_Y),Vec2(1.f,1.f));
	m_player1=new Player(this,true,m_map);
	m_player2=new Player(this,false,m_map);
	m_map->Startup();
	m_player1->Startup();
	m_player2->Startup();
}
void Game::Shutdown()
{
	
}

void Game::RunFrame(float deltaTime)
{
	ControlCollision();
	Update(deltaTime);
}

const void Game::Render()
{
	m_map->RenderTiles();
	
	m_player1->Render();
	if( m_isAttractMode ) {

	}
	else{
	}
}

void Game::ControlCollision()
{

}

void Game::RenderUI()
{
	
	
}

void Game::Reset()
{	
	
	Startup();
	
}



void Game::Update(float deltaTime)
{
	static Player* temPlayer;
	if(m_isFirstPlayer){
		temPlayer=m_player1;
	}
	else{
		temPlayer=m_player2;
	}
	switch (m_gameStage){
	case TURN_START:
		m_gameStage=PLAYER_MOVE;
		InitialPlayer(temPlayer);
		break;
	case PLAYER_MOVE:
		MovePlayer(temPlayer);
		break;
	case PLAYER_SPREAD:
		temPlayer->SpreadGrids();
		m_map->SpreadGrids();
		m_gameStage=TURN_END;
		break;
	case TURN_END:
		CheckSuccess();
		break;
	}
	
	UpdateMap();
	
}


void Game::InitialPlayer(Player* inPlayer)
{
	inPlayer->InitialEachTurn();
	m_map->SetPlayerPos(*inPlayer);
}




void Game::UpdateMap()
{
	m_map->Update();
}

void Game::CheckSuccess()
{
	bool isSuccessful=	m_map->CheckSuccess();
	if(isSuccessful){
		m_gameStage=GAME_END;
	}
	else{
		m_isFirstPlayer=!m_isFirstPlayer;
		m_gameStage=TURN_START;
	}
}

void Game::MovePlayer(Player* inPlayer)
{
	bool isLeft=false;
	bool isRight=false;
	bool isUp=false;
	bool isDown=false;
	if(m_theInput->WasKeyJustPressed(KEYBOARD_BUTTON_ID_LEFT_ARROW)){
		isLeft=true;
	}
	bool testing=m_theInput->WasKeyJustPressed( KEYBOARD_BUTTON_ID_RIGHT_ARROW );
	if(m_theInput->WasKeyJustPressed( KEYBOARD_BUTTON_ID_RIGHT_ARROW ) ) {
		isRight=true;
	}
	if(m_theInput->WasKeyJustPressed( KEYBOARD_BUTTON_ID_UP_ARROW ) ) {
		isUp=true;
	}
	if( m_theInput->WasKeyJustPressed( KEYBOARD_BUTTON_ID_DOWN_ARROW ) ) {
		isDown=true;
	}
	if(isLeft||isRight||isDown||isUp){
		inPlayer->Move(isUp,isDown,isRight,isLeft);
		m_map->SetPlayerPos(*inPlayer);
	}
	if(m_theInput->WasKeyJustPressed(KEYBOARD_BUTTON_ID_A)){
		m_map->SelectGridByPlayer(*inPlayer);
		m_gameStage=PLAYER_SPREAD;
	}
}



