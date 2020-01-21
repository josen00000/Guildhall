#include <windows.h>
#include "Game.hpp"
#include "Game/World.hpp"
#include "Game/App.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"


extern App* g_theApp;
extern InputSystem* g_theInputSystem;
const XboxController* playerController = &(g_theInputSystem->GetXboxController(0));
extern AudioSystem* g_theAudioSystem;
extern RenderContext* g_theRenderer;
extern BitmapFont* g_squirrelFont;
extern Camera* g_camera;
extern Camera* g_consoleCamera;
Texture* temTexture = nullptr;
SpriteSheet* temSpriteSheet = nullptr;


Game::Game( bool isDevelop,Camera* inCamera)
	:m_developMode(isDevelop)
	,m_camera(inCamera)
{
	m_rng=RandomNumberGenerator();
	playerController=&g_theInputSystem->GetXboxController(0);
	
}

void Game::Startup()
{	
	m_world = new World();
	CreatePauseScene();
	
}
void Game::Shutdown()
{
	m_world->DestroyEntities();
}

void Game::RunFrame(float deltaSeconds)
{
	Update(deltaSeconds);
}

void Game::CreatePauseScene()
{
	m_pauseColor = Rgba8(0,0,0,100);
	m_pauseScene = g_camera->m_AABB2;
	g_squirrelFont->AddVertsForText2D(m_pauseText,Vec2( 3, 5 ), 0.5, "press p to start");
}

void Game::CheckGameStates()
{
	
	CheckIfPause();
	CheckIfDeveloped();
	CheckIfDebugNoClip();
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F4 ) ) {
		m_debugCamera=!m_debugCamera;
		if(m_debugCamera){
			m_camera->SetOrthoView(Vec2(0,0),Vec2((float)(30*1.67),30.f));
		}
		else{
			m_camera->SetOrthoView(Vec2( 0, 0 ), Vec2( 16, 9 ) );
		}
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F8 ) ) {
		g_theApp->ResetGame();
	}
}

void Game::CheckIfPause()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P)){
		if( !m_isPlayerDead ) {
			m_isPause=!m_isPause;
		}
	}
	if( m_isPause ) {
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) ) {
				m_gameState = GAME_STATE_ATTRACT;
				m_resetFinish =false;

		}
		
	}
	
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) ) {
		if( !m_isPause && !m_world->m_isWin ) {
			g_theApp->HandleQuitRequested();

		}
		
	}
}

void Game::CheckIfDeveloped()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F1 ) ) {
		m_developMode=!m_developMode;
	}
}

void Game::CheckIfDebugNoClip()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F3 ) ) {
		m_noClip=!m_noClip;
	}
}

/*
void Game::TestDevConsole()
{

	m_testConsole = new DevConsole( g_squirrelFont );
	for( int i = 0; i < 10; i++ ) {
		m_testConsole->PrintString( Rgba8::WHITE, "testing" );

	}
	for( int i = 0; i < 10; i++ ) {
		m_testConsole->PrintString( Rgba8::WHITE, "after testing" );


	}
	for( int i = 0; i < 10; i++ ) {
		m_testConsole->PrintString( Rgba8::WHITE, " not after testing" );

	}
}*/

/*
void Game::RenderTestDevConsole() const
{
	m_testConsole->Render(*g_theRenderer, *g_consoleCamera, 1);
}*/

void Game::Render() const
{
	switch( m_gameState )
	{
	case GAME_STATE_LOADING:
		RenderLoadingScene();
		break;
	case GAME_STATE_ATTRACT:
		RenderAttractScene();
		break;
	case GAME_STATE_PLAYING:
		m_world->Render();
		if(m_isPause){
			RenderPauseScene();
		}
		break;
	case GAME_STATE_VICTORY:
		break;
	case GAME_STATE_PAUSED:
		break;
	case NUM_GAME_STATE:
		break;
	default:
		break;
	}
}



void Game::RenderUI() const
{
	m_world->RenderUI();
}

void Game::RenderLoadingScene() const
{
	g_theRenderer->BindTexture(g_squirrelFont->GetTexture());
	g_theRenderer->DrawVertexVector(m_LoadingTextVertices);
}

void Game::RenderAttractScene() const
{
	g_theRenderer->BindTexture( g_squirrelFont->GetTexture() );
	g_theRenderer->DrawVertexVector( m_attractTextVertices );
}

void Game::RenderPauseScene() const
{
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawAABB2D( m_pauseScene,m_pauseColor );
	g_theRenderer->BindTexture(g_squirrelFont->GetTexture());
	g_theRenderer->DrawVertexVector(m_pauseText);
}


void Game::Reset()
{	
	Startup();
	m_developMode = false;
	m_noClip = false;
	m_isPause = false;
	m_isPlayerDead = false;
	m_debugCamera = false;
	isAppQuit = false;
	m_isLoadingSceneRender = false;
	m_resetFinish = true;
}

void Game::UpdateLoadingScene( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	m_LoadingTextVertices.clear();
	g_squirrelFont->AddVertsForText2D( m_LoadingTextVertices, Vec2( 3, 5 ), 0.5, "Loading....." );
	m_isLoadingSceneRender = true;
}

void Game::UpdateAttractScene( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	m_attractTextVertices.clear();
	g_squirrelFont->AddVertsForText2D( m_attractTextVertices, Vec2( 3, 5 ), 0.5, "Press space to play." );
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_SPACE ) ) {
		m_gameState = GAME_STATE_PLAYING;
	}
}

void Game::Update(float deltaSeconds)
{
	CheckIfExit();
	switch( m_gameState )
	{
	case GAME_STATE_LOADING:
		if(m_isLoadingSceneRender){
			LoadGameAsset();
			m_gameState = 	GAME_STATE_ATTRACT;
			m_resetFinish = false;
		}	
		else{
			UpdateLoadingScene(deltaSeconds);
		}
		break;
	case GAME_STATE_ATTRACT:
		if(!m_resetFinish){
			Reset();
		}
		UpdateAttractScene(deltaSeconds);
		break;
	case GAME_STATE_PLAYING:
		CheckGameStates();
		if( m_isPause ) { return; }
		UpdateWorld( deltaSeconds );
		break;
	case GAME_STATE_VICTORY:
		break;
	case GAME_STATE_PAUSED:
		break;
	}

}






void Game::UpdateWorld( float deltaSeconds )
{
	m_world->Update(deltaSeconds);
}

void Game::LoadGameAsset()
{
	//Startup();
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Bullet.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Explosion_5x5.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/FriendlyTank1.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTank4.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTurretBase.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTurretTop.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTank1.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_SpriteSheet8x2.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankTop.png" );
	
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/PlayerShootNormal.ogg" );
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/PlayerHit.wav" );
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/PlayerDied.wav" );
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyShoot.wav" );
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyHit.wav" );
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyDied.wav" );


	//load every texture and sound.
}

