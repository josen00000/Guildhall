#include <windows.h>
#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/XmlUtils.hpp"
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
extern Camera* g_UICamera;
extern Camera* g_consoleCamera;
Texture* temTexture = nullptr;
SpriteSheet* temSpriteSheet = nullptr;


Game::Game( bool isDevelop,Camera* inCamera)
	:m_developMode(isDevelop)
	,m_camera(inCamera)
{
	m_rng=RandomNumberGenerator();
	playerController = &g_theInputSystem->GetXboxController(0);
	
}

void Game::Startup()
{	
	LoadDefFromXmlFile();
	m_world = new World();
	
}

void Game::LoadDefFromXmlFile()
{
	//Load tile Def
	XmlDocument tileDefDocument;
	tileDefDocument.LoadFile("Data/Definitions/Tiles.xml");
	if(IfLoadXmlFileSucceed(tileDefDocument)){
		XmlElement* rootElement = tileDefDocument.RootElement();
		XmlElement* childElement = rootElement->FirstChildElement();
		
		TileDefinition::ParseSpriteSheetFromXmlElement( *rootElement );
		while(childElement){
			TileDefinition::PopulateDefinitionsFromXmlElement(*childElement);
			childElement = childElement->NextSiblingElement();
		}
	
	}

	//Load map Def
	XmlDocument mapDefDocument;
	mapDefDocument.LoadFile( "Data/Definitions/Maps.xml" );
	if( IfLoadXmlFileSucceed( mapDefDocument ) ) {
		XmlElement* rootElement = mapDefDocument.RootElement();
		XmlElement* childElement = rootElement->FirstChildElement();

		while( childElement ) {
			MapDefinition::PopulateDefinitionsFromXmlElement( *childElement );
			childElement = childElement->NextSiblingElement();
		}

	}

	//Load Actor Def
	XmlDocument actorDefDocument;
	actorDefDocument.LoadFile( "Data/Definitions/Actors.xml" );
	if( IfLoadXmlFileSucceed( actorDefDocument )){
		XmlElement* rootElement = actorDefDocument.RootElement();
		XmlElement* childElement = rootElement->FirstChildElement();

		while( childElement ){
			ActorDefinition::PopulateDefinitionsFromXmlElement( *childElement );
			childElement = childElement->NextSiblingElement();
		}
	}

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



void Game::UpdateDisplayCursorInfo( Map* map )
{
	m_displayInfo.clear();
	m_displayVertices.clear();
	m_displayInfo = map->GetDisplayInfo();
	AABB2 disPlayBox = AABB2( 0, 70, 20, 90 );
	float alignheight = 0;
	for(int infoIndex = 0; infoIndex < m_displayInfo.size(); infoIndex++ ){
		g_squirrelFont->AddVertsForTextInBox2D( m_displayVertices, disPlayBox, 1.5, m_displayInfo[0], Rgba8::WHITE, 1, Vec2( 0, alignheight ) ); 
		alignheight += 0.1f;
	}
}

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
	//m_world->RenderUI();
	RenderCursor();
	RenderDisplay();
}

void Game::RenderCursor() const
{
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawCircle( m_mousePos, 3, 1, Rgba8::RED );
}

void Game::RenderDisplay() const
{
	g_theRenderer->BindTexture( g_squirrelFont->GetTexture() );
	g_theRenderer->DrawVertexVector( m_displayVertices );
	
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
			//TestDevConsole();
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
		UpdateCursor( deltaSeconds );
		UpdateDisplayCursorInfo( m_world->m_maps[0] );
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

void Game::UpdateCursor( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	m_mousePos = g_theInputSystem->GetNormalizedMousePosInCamera( *g_UICamera );
}

void Game::LoadGameAsset()
{
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_32x32.png" );
	//load every texture and sound.
}

