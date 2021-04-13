#include "game.hpp"
#include "Game/App.hpp"
#include "../../Thesis/Code/Game/Camera/CameraSystem.hpp"
#include "Game/Map/MapDefinition.hpp"
#include "Game/Map/TileDefinition.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Player.hpp"
#include "Game/World.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/DebugRender.hpp"

//////////////////////////////////////////////////////////////////////////
// global variable
//////////////////////////////////////////////////////////////////////////
// Game
//////////////////////////////////////////////////////////////////////////
extern App*				g_theApp;
extern CameraSystem*	g_theCameraSystem;
extern Game*			g_theGame;

// Engine
//////////////////////////////////////////////////////////////////////////
extern InputSystem*		g_theInputSystem;
extern BitmapFont*		g_squirrelFont;
extern RenderContext*	g_theRenderer;
extern DevConsole*		g_theConsole;	

//////////////////////////////////////////////////////////////////////////
// public
//////////////////////////////////////////////////////////////////////////

// Constructor
//////////////////////////////////////////////////////////////////////////
Game::Game( Camera* gameCamera, Camera* UICamera )
	:m_gameCamera(gameCamera)
	,m_UICamera(UICamera)
{
}
//////////////////////////////////////////////////////////////////////////


// Game Flow
//////////////////////////////////////////////////////////////////////////
void Game::Startup()
{
	LoadAssets();
	LoadDefs();
	m_world = World::CreateWorld( 1 );
	g_theCameraSystem = new CameraSystem();
	g_theCameraSystem->Startup();

	g_theCameraSystem->SetSplitScreenState( AXIS_ALIGNED_SPLIT );
	g_theCameraSystem->SetCameraShakeState( BLEND_SHAKE );
	

	g_theConsole->AddCommandToCommandList( std::string( "set_asymptotic"), std::string( "Asymptotic value for camera controller" ), SetCameraAsymptoticValue );
	g_theConsole->AddCommandToCommandList( std::string( "spawn_item"), std::string( "spawn new item" ), SpawnItem );
	g_theConsole->AddCommandToCommandList( std::string( "delete_player"), std::string( "delete player with index" ), DeletePlayer );
	g_theConsole->AddCommandToCommandList( std::string( "set_camera_smooth" ), std::string( "set is controller smooth motion" ), SetIsSmooth );
}

void Game::Shutdown()
{
	delete m_gameCamera;
	delete m_UICamera;
	delete m_explodeSpriteSheet;
	delete m_explodeAnimDef;

	m_explodeSpriteSheet = nullptr;
	m_explodeAnimDef	= nullptr;
	m_gameCamera		= nullptr;
	m_UICamera			= nullptr;
}

void Game::RunFrame( float deltaSeconds )
{
	if( !g_theConsole->IsOpen() ) {
		HandleInput();
	}
	if( m_isPaused ) {
		deltaSeconds = 0.f;
	}
	UpdateGame( deltaSeconds );
	UpdateUI( deltaSeconds );

}

void Game::RenderGame() const
{
	m_world->RenderWorld();
}

void Game::RenderUI() const
{
	if( m_world->GetCurrentMap()->GetPlayerNum() == 2 ) {
		Strings hpStrs;
		hpStrs.push_back( std::string( "player 1 hp:" + std::to_string( m_world->GetCurrentMap()->GetPlayerWithIndex( 0 )->GetHealth())) );
		hpStrs.push_back( std::string( "player 2 hp:" + std::to_string( m_world->GetCurrentMap()->GetPlayerWithIndex( 1 )->GetHealth())) );

		DebugAddScreenLeftAlignStrings( 0.9f, -5.f, Rgba8::WHITE, hpStrs );
	}
}

void Game::EndFrame()
{
	m_world->EndFrame();
}

//////////////////////////////////////////////////////////////////////////


Map* Game::GetCurrentMap()
{
	return m_world->GetCurrentMap();
}

// Mutator
//////////////////////////////////////////////////////////////////////////
void Game::SetIsDebug( bool isDebug )
{
	m_isDebug = isDebug;
	g_theCameraSystem->SetIsDebug( isDebug );
}

void Game::SetIsPaused( bool isPaused )
{
	m_isPaused = isPaused;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// private
//////////////////////////////////////////////////////////////////////////

// Game flow
//////////////////////////////////////////////////////////////////////////
void Game::HandleInput()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F1 ) ) {
		bool isdebug = GetIsDebug();
		SetIsDebug( !isdebug );
	}
// 	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P ) ) {
// 		Map* currentMap = m_world->GetCurrentMap();
// 		if( currentMap != nullptr ) {
// 			currentMap->CreatePlayer();
// 		 }
// 	}
	
	
}

void Game::UpdateGame( float deltaSeconds )
{
	// check exit
	// update world
	CheckIfExit();


	m_world->UpdateWorld( deltaSeconds );
	g_theCameraSystem->Update( deltaSeconds );
}


void Game::UpdateUI( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::RenderGameUI() const
{

}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) || g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_X ) ) {
		g_theApp->HandleQuitRequested();
	}
}



//////////////////////////////////////////////////////////////////////////


// loading
//////////////////////////////////////////////////////////////////////////
void Game::LoadAssets()
{
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "squirrelFont", "Data/Fonts/SquirrelFixedFont" );	
	m_playerTexture				= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	m_playerBarrelTexture		= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankTop.png" );
	m_enemyTexture				= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTank1.png" );
	m_enemyBarrelTexture		= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyCannon.png" );
	m_playerProjectileTexture	= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/FriendlyBullet.png" );
	m_enemyProjectileTexture	= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyBullet.png" );
	m_aimTexture				= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Aim.png");
	Texture* explosionTexture	= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Explosion_5x5.png" );
	m_explodeSpriteSheet		= new SpriteSheet( explosionTexture, IntVec2( 5, 5 ));
	std::vector<int> spriteIndexes = std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,19,20,21,22,23,24 };
	m_explodeAnimDef = new SpriteAnimDefinition( *m_explodeSpriteSheet, spriteIndexes.data(), (int)spriteIndexes.size(), 0.1f, SpriteAnimPlaybackType::ONCE );
}

void Game::LoadDefs()
{
	MapDefinition::PopulateDefinitionsFromXmlFile( MAP_DEF_FILE_PATH );
	TileDefinition::PopulateDefinitionFromXmlFile( TILE_DEF_FILE_PATH );
	//ActorDefinition::PopulateDefinitionFromXmlFile( ACTOR_DEF_FILE_PATH );
}
//////////////////////////////////////////////////////////////////////////

bool SetCameraAsymptoticValue( EventArgs& args )
{
	float asymptoticValue = args.GetValue( std::to_string( 0 ), 0.f );
	std::vector<CameraController*> controllers = g_theCameraSystem->GetCameraControllers();
	for( int i = 0; i < controllers.size(); i++ ) {
		controllers[i]->SetAsymptoticValue( asymptoticValue );
	}
	return true;
}

bool SpawnItem( EventArgs& args )
{
	UNUSED(args);
	//Vec2 pos = args.GetValue( std::to_string( 0 ), Vec2::ZERO );
	//Map* currentMap = g_theGame->m_world->GetCurrentMap();
	//currentMap->SpawnNewItem( pos );
	return true;
}

bool DeletePlayer( EventArgs& args )
{
	std::string playerIndex = args.GetValue( std::to_string( 0 ), "0" );
	
	Map* currentMap = g_theGame->GetCurrentMap();
	currentMap->DestroyPlayerWithIndex( atoi(playerIndex.c_str()) );
	return true;
}

bool SetIsSmooth( EventArgs& args )
{
	std::string isSmoothText = args.GetValue( std::to_string( 0 ), std::string() );
	bool isSmooth = GetBoolFromText( isSmoothText.c_str() );
	g_theCameraSystem->SetControllerSmooth( isSmooth );
	return true;
}
