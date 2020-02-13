#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/World.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern App*				g_theApp;
extern BitmapFont*		g_squirrelFont;
extern AudioSystem*		g_theAudioSystem; 
extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInputSystem;


Game::Game( Camera* gameCamera, Camera* UICamera )
	:m_gameCamera(gameCamera)
	,m_UICamera(UICamera)
{
	m_rng = new RandomNumberGenerator( 0 );
}

void Game::Startup()
{
	m_world = new World();
	m_gameState = GAME_STATE_LOADING;
	m_developMode	= false;
	m_noClip		= false;
	m_isPause		= false;
	m_debugCamera	= false;
	m_isAppQuit		= false;
	m_isAttractMode	= false;
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_world;
}

void Game::RunFrame( float deltaSeconds )
{
	Update( deltaSeconds );
}

void Game::Reset()
{
	delete m_world;
	m_world = nullptr;
	Startup();
}

void Game::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
 	CheckIfExit();
	m_gameCamera->SetClearMode( CLEAR_COLOR_BIT, Rgba8::RED, 0.0f, 0 );

	Vec2 movement = Vec2::ZERO;
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_LEFT_ARROW ) ) {
		movement.x -= 1.0f;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_RIGHT_ARROW ) ) {
		movement.x += 1.0f;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_UP_ARROW ) ) {
		movement.y += 1.0f;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_DOWN_ARROW ) ) {
		movement.y -= 1.0f;
	}
	Vec3 cameraPos = m_gameCamera->GetPosition();
	cameraPos += movement;
	m_gameCamera->SetPosition( cameraPos );

// 	switch ( m_gameState )
// 	{
// 		case GAME_STATE_LOADING:{
// 			LoadAssets();
// 			m_gameState = GAME_STATE_ATTRACT;
// 			break;
// 		}
// 		case GAME_STATE_ATTRACT:{
// 			static bool isResetFinished = false;
// 			if( !isResetFinished ){
// 				Reset();
// 				isResetFinished = true;
// 			}
// 			if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_SPACE)){
// 				m_gameState = GAME_STATE_PLAYING;
// 			}
// 			break;
// 		}
// 		case GAME_STATE_PLAYING:{
// 			if( m_isPause ){ return; }
// 			m_world->Update( deltaSeconds );
// 			break;
// 		}
// 		case GAME_STATE_END:{
// 			break;
// 		}
// 
// 
// 	}
}

void Game::UpdateUI( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) || g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_X ) ){
		g_theApp->HandleQuitRequested();
	}
	if( g_theWindow->m_isWindowClose ){
		g_theApp->HandleQuitRequested();
	}
}

void Game::CheckGameStates()
{
	CheckIfPause();
	CheckIfDeveloped();
	CheckIfNoClip();
}

void Game::CheckIfPause()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P ) ){
		m_isPause = !m_isPause;
	}
}

void Game::CheckIfDeveloped()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F1 )){
		m_developMode = !m_developMode;
	}
}

void Game::CheckIfNoClip()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F3 )){
		m_noClip = !m_noClip;
	}
}

void Game::Render() const
{
	//m_world->Render(); past code
	static int tempSelect = 0;
	static Rgba8 tempColor;
	switch( tempSelect )
	{
	case 1: tempColor = Rgba8::BLACK;
		break;
	case 2: tempColor = Rgba8::BLACK;
		break;
	case 3: tempColor = Rgba8::BLACK;
		break;
	case 4: tempColor = Rgba8::BLACK;
		break;
	case 5: tempColor = Rgba8::BLACK;
		break;
	case 6: tempColor = Rgba8::GRAY;
		break;
	case 7: tempColor = Rgba8::GRAY;
		break;
	case 8: tempColor = Rgba8::GRAY;
		break;
	case 9: tempColor = Rgba8::GRAY;
		break;
	case 10: tempColor = Rgba8::GRAY;
		break;
	}
	if( tempSelect < 10 ) {
		tempSelect++;
	}
	else {
		tempSelect = 0;
	}
	tempColor = Rgba8::GRAY;
	m_gameCamera->m_clearColor = tempColor;
	g_theRenderer->BeginCamera( *m_gameCamera );
	//g_theRenderer->Draw( 3, 0 );
	//g_theRenderer->DrawLine( Vec2::ZERO, Vec2( 0 , 5), 0.5f, Rgba8::RED );
	g_theRenderer->DrawAABB2D( AABB2( Vec2::ZERO, Vec2(5,5) ), Rgba8::RED );
	g_theRenderer->EndCamera( *m_gameCamera );
}

void Game::RenderUI() const
{
	g_theRenderer->DrawVertexVector( m_UIVertices );
}

void Game::LoadAssets()
{
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	//g_theRenderer->CreateOrGetTextureFromFile("FilePath");
	//g_theAudioSystem->CreateOrGetSound("FilePath");
}

