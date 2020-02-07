#include <windows.h>
#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern HWND g_hWnd;

extern App*				g_theApp;
extern BitmapFont*		g_squirrelFont;
extern AudioSystem*		g_theAudioSystem; 
extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInputSystem;
extern DevConsole*		g_theConsole;

Game::Game( Camera* gameCamera )
	:m_gameCamera(gameCamera)
{
	m_rng = new RandomNumberGenerator( 0 );
}

void Game::Startup()
{
	//CreateGameObject();
	BitmapFont* m_testFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	g_theConsole = new DevConsole( m_testFont );
}

void Game::Shutdown()
{
	delete m_rng;
}

void Game::RunFrame( float deltaSeconds )
{
	Update( deltaSeconds );
	
}

void Game::EndFrame()
{

}

void Game::Reset()
{
	Startup();
}

void Game::Update( float deltaSeconds )
{
	CheckIfExit();
	UpdateCamera( deltaSeconds );
	UpdateMouse( deltaSeconds );
	UpdateGameObjects( deltaSeconds );

	if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_LEFT ) ) {
		if( !SelectGameObject() ) {
			CreateGameObject();
			m_isDragging = false;
		}
		else {
			m_isDragging = true;
			m_selectOffset = m_mousePos - m_selectedObj->GetPosition();
		}
	}

	if( g_theInputSystem->WasMouseButtonJustReleased( MOUSE_BUTTON_LEFT ) ) {
		// finish drag and unselect
		m_isDragging = false;
		if( m_selectedObj != nullptr ) {
			m_selectedObj->SetIsSelected( false );
			m_selectedObj = nullptr;
		}
	}
// 	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_T ) ) {
// 		CreateGameObject();
// 	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F ) ) {
		g_theConsole->SetIsOpen( true );
		g_theConsole->PrintString( Rgba8::RED, std::string("isopen"));
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_C ) ) {
		g_theConsole->SetIsOpen( false );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_O ) ) {
		SetCameraToOrigin();
	}
	
		
}

bool Game::SelectGameObject()
{
	if( m_gameObjects.size() == 0 ){ return false; }

	for( int objIndex = (int)m_gameObjects.size() - 1; objIndex >= 0; objIndex-- ) {
		if( m_gameObjects[objIndex] == nullptr ){ continue; }
		if( m_gameObjects[objIndex]->GetIfMouseIn() ) {
			m_selectedObj = m_gameObjects[objIndex];
			m_selectedObj->SetIsSelected( true );
			return true;
		}
	}
	return false;
}

void Game::UpdateGameObjects( float deltaSeconds )
{
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		GameObject* gameObj = m_gameObjects[objIndex];
		if( gameObj == nullptr ){ continue; }
		m_gameObjects[objIndex]->Update( deltaSeconds );
	}
	UpdateGameObjectsTouching();
}

void Game::UpdateCamera( float deltaSeconds )
{
	UpdateCameraPos( deltaSeconds );
	UpdateCameraHeight( deltaSeconds );
}

void Game::UpdateCameraHeight( float deltaSeconds )
{
	// get mouse scroll value
	// range map from 0 to 100
	// set height from 100 to 10
	
	UNUSED(deltaSeconds);
	float clampedScroll = ClampFloat( 0.f, 100.f, m_mouseScroll );
	float height = RangeMapFloat( 0.f, 100.f, 100.f, 50.f, clampedScroll );
	std::string mouseScroll = "The height is " + std::to_string( height );
	g_theConsole->PrintString( Rgba8::RED, mouseScroll );

	m_gameCamera->SetProjectionOrthographic( height );
}

void Game::UpdateCameraPos( float deltaSeconds )
{
	// Get input
	// Get velocity of the camera
	// apply the translation to camera
	m_cameraDirection = Vec2::ZERO;
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_W ) ) {
		m_cameraDirection = Vec2( 0, 1 );
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_A ) ) {
		m_cameraDirection = Vec2( -1, 0 );
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
		m_cameraDirection = Vec2( 1, 0 );
	}
	else if ( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S )) {
		m_cameraDirection = Vec2( 0, -1 );
	}
	Vec2 cameraVelocity = m_cameraDirection * m_cameraSpeed;
	Vec2 cameraDisp = cameraVelocity * deltaSeconds;
	Vec2 currentCameraPos = m_gameCamera->GetPosition(); 
	m_gameCamera->SetPosition( currentCameraPos + cameraDisp );

}

void Game::SetCameraToOrigin()
{
	m_gameCamera->SetOrthoView( Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ) );
	g_theInputSystem->ResetMouseWheel();
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) && !m_isDragging ){
		g_theApp->HandleQuitRequested();
	}
}

void Game::UpdateMouse( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	UpdateMousePos();
	UpdateMouseWheel();
}

void Game::UpdateMouseWheel()
{
	m_mouseScroll = g_theInputSystem->GetMouseWheelAmount();
}

void Game::UpdateMousePos()
{
	// Get Mouse Pos;
	//m_mousePos = g_theInputSystem->GetNormalizedMousePosInCamera( *m_gameCamera );
	Vec2 clientPos = g_theInputSystem->GetNormalizedMousePos();
	
	POINT screenMousePos;
	GetCursorPos( &screenMousePos );
	ScreenToClient( g_hWnd, &screenMousePos );
	Vec2 mouseClientPos( (float)screenMousePos.x, (float)screenMousePos.y );
	m_mousePos = m_gameCamera->ClientToWorldPosition( mouseClientPos );

	if( m_isDragging ) {
		if( m_selectedObj != nullptr ) {
			m_selectedObj->SetPosition( m_mousePos - m_selectOffset );
		}
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) || g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_DELETE ) ) {
			m_selectedObj->SetIsSelected( false );
			DeleteGameObject( m_selectedObj );
			m_selectedObj = nullptr;
		}
	}

	//std::string mousePos = "The mouse position is " + std::to_string( m_mousePos.x ) + " " + std::to_string( m_mousePos.y );
	//g_theConsole->PrintString( Rgba8::RED, mousePos );
}

void Game::RenderMouse() const
{
	g_theRenderer->DrawCircle( m_mousePos, 1, 1, Rgba8::WHITE );
}

void Game::Render() const
{
	BitmapFont* font = g_theConsole->m_font;
	g_theRenderer->BindTexture( font->GetTexture() );
	g_theConsole->Render( *g_theRenderer, *m_gameCamera, 1 );

	g_theRenderer->BindTexture( nullptr );
	//RenderMouse();
	RenderGameObjects();

}


void Game::RenderGameObjects() const
{
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		if( m_gameObjects[objIndex] == nullptr ) { continue; }
		m_gameObjects[objIndex]->Render();
	}
}

void Game::CreateGameObject()
{
	// get cursor position
	// create the game object 
	float tempRadius = m_rng->RollRandomFloatInRange( 5 , 15 );
	GameObject* tempGameObj = new GameObject( m_mousePos, tempRadius );
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		if( m_gameObjects[objIndex] == nullptr ) {
			m_gameObjects[objIndex] = tempGameObj;
			return;
		}
	}
	m_gameObjects.push_back( tempGameObj );
	g_theConsole->PrintString( Rgba8::RED, std::to_string( m_gameObjects.size() ) );
}


void Game::DeleteGameObject( GameObject* obj )
{
	if( obj == nullptr ){ return; }

	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		if( m_gameObjects[objIndex] == obj ) {
			delete m_gameObjects[objIndex];
			m_gameObjects[objIndex] = nullptr;
		}
	}
}

void Game::UpdateGameObjectsTouching()
{
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		if( m_gameObjects[objIndex] == nullptr ){ continue; }
		GameObject* tempObj = m_gameObjects[objIndex];
		tempObj->SetIsTouching( false );
		if( tempObj == nullptr ) { continue; }

		for( int objIndex1 = 0; objIndex1 < m_gameObjects.size(); objIndex1++ ) {
			if( m_gameObjects[objIndex1] == nullptr ){ continue; }
			GameObject* tempObj1 = m_gameObjects[objIndex1];
			if( tempObj1 == nullptr || objIndex1 == objIndex ){ continue; }
			// Check if touching
			
			if( tempObj1->IsIntersectWith( tempObj ) ) {
				tempObj->SetIsTouching( true );
				tempObj1->SetIsTouching( true );
			}
		}
	}
}

