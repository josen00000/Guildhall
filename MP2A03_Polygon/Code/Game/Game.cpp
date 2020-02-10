#include <windows.h>
#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

extern App*				g_theApp;
extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInputSystem;
extern HWND				g_hWnd;
extern DevConsole*		g_theConsole;
extern Physics2D*		g_thePhysics;

Game::Game( Camera* gameCamera, Camera* UICamera )
	:m_gameCamera(gameCamera)
	,m_UICamera(UICamera)
{
	m_rng = new RandomNumberGenerator( 0 );
}

void Game::Startup()
{
	m_isAppQuit		= false;
	//BitmapFont* testFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	//g_theConsole = new DevConsole( testFont );
	//
	//Vec2 polyPoints[5] ={ Vec2( 10, 10 ), Vec2( 20, 10 ), Vec2( 20, 20 ), Vec2( 15, 15 ), Vec2( 10, 20 )};
	GenerateTempPoints();
	GenerateTestPoints();
	Polygon2 tempPoly = Polygon2::MakeConvexFromPointCloud( m_tempPoints );
	//GameObject* polyTest = new GameObject( Vec2( 10, 10 ), tempPoly );
	//m_gameObjects.push_back( polyTest );
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_gameCamera;
	delete m_UICamera;
}

void Game::RunFrame( float deltaSeconds )
{
	Update( deltaSeconds );
}

void Game::Update( float deltaSeconds )
{
	CheckIfExit();
	UpdateMouse();
	UpdatePhysics( deltaSeconds );


}

void Game::UpdatePhysics( float deltaSeconds )
{
	g_thePhysics->Update( deltaSeconds );
}

void Game::UpdateUI( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::UpdateMouse()
{
	UpdateMouseWheel();
	UpdateMousePos();
	HandleMouseInput();
	HandleKeyboardInput();
}

void Game::UpdateMouseWheel()
{
	m_mouseScroll = g_theInputSystem->GetMouseWheelAmount();
}

void Game::UpdateMousePos()
{
	Vec2 clientPos = g_theInputSystem->GetNormalizedMousePos();

	POINT screenMousePos;
	GetCursorPos( &screenMousePos );
	ScreenToClient( g_hWnd, &screenMousePos );
	Vec2 mouseClientPos( (float)screenMousePos.x, (float)screenMousePos.y );
	m_mousePos = m_gameCamera->ClientToWorldPosition( mouseClientPos );

	// handle drag object
	if( m_isDragging ) {
		if( m_selectedObj != nullptr ) {
			m_selectedObj->m_position = ( m_mousePos - m_selectOffset );
		}

	}
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) ){
		if( m_isDrawMode ) {
			CancelDrawPolygon();
		}
		else if( m_isDragging ){}
		else{
			g_theApp->HandleQuitRequested();
		}
	}
}

void Game::UpdateCamera( float deltaSeconds )
{
	UpdateCameraPos( deltaSeconds );
	UpdateCameraHeight( deltaSeconds );
}

void Game::UpdateCameraHeight( float deltaSeconds )
{

}

void Game::UpdateCameraPos( float deltaSeconds )
{
	m_cameraMoveDirct = Vec2::ZERO;
	
	Vec2 cameraVelocity = m_cameraMoveDirct * m_cameraMoveSpeed;
	Vec2 cameraDisp = cameraVelocity * deltaSeconds;
	Vec2 currentCameraPos = m_gameCamera->GetPosition();
	m_gameCamera->SetPosition( currentCameraPos + cameraDisp );
}

void Game::SetCameraToOrigin()
{

}

void Game::HandleMouseInput()
{
	if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_LEFT ) ){
		if( m_isDrawMode ){
			AddPointToDrawPolygon( m_mousePos );
		}
		else if( SelectGameObject() ) {
			m_isDragging = true;
			m_selectOffset = m_mousePos - m_selectedObj->m_position;
		}
	}
	if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_RIGHT ) ) {
		EndDrawPolygon();
	}

	if( g_theInputSystem->WasMouseButtonJustReleased( MOUSE_BUTTON_LEFT ) ) {
		m_isDragging = false;
		if( m_selectedObj != nullptr ) {
			m_selectedObj->m_isSelected = false;
			m_selectedObj = nullptr;
		}
	}

	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F ) ) {
		//g_theConsole->SetIsOpen( true );
		//g_theConsole->PrintString( Rgba8::RED, std::string( "isopen" ) );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_C ) ) {
		//g_theConsole->SetIsOpen( false );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_O ) ) {
		SetCameraToOrigin();
	}

}

void Game::HandleKeyboardInput()
{
	// draw input
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_2 ) ) {
		if( !m_isDrawMode ){
			BeginDrawPolygon();
		}
	}

	// camera move
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_W ) ) {
		m_cameraMoveDirct = Vec2( 0, 1 );
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_A ) ) {
		m_cameraMoveDirct = Vec2( -1, 0 );
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
		m_cameraMoveDirct = Vec2( 1, 0 );
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {
		m_cameraMoveDirct = Vec2( 0, -1 );
	}


}

void Game::BeginDrawPolygon()
{
	m_isDrawMode = true;
	Vec2 start = m_mousePos;
	m_drawPoints.clear();
	m_drawPoints.push_back( start );
}

void Game::EndDrawPolygon()
{
	m_isDrawMode = false;
	if( IsPointEnoughForDrawPolygon() ){
		// Create polygon with point
		GameObject* obj = new GameObject( m_drawPoints );
		m_gameObjects.push_back( obj );
		m_drawPoints.clear();
	}
}

void Game::CancelDrawPolygon()
{
	m_isDrawMode = false;
	m_drawPoints.clear();
}

bool Game::IsPointEnoughForDrawPolygon() const
{
	if( m_drawPoints.size() > 3 ){ 
		return true;
	}
	else {
		return false;
	}
}

bool Game::IsDrawedPolygonConvex() const
{
	
	std::vector<Vec2> tempPoints;
	tempPoints = m_drawPoints;
	tempPoints.push_back( m_mousePos );
	Polygon2 tempPoly = Polygon2( tempPoints );
	if( !tempPoly.IsConvex() ) {
		return false;
	}
	else{
		return true;
	}
}

void Game::Render() const
{
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		m_gameObjects[objIndex]->Render();
	}
// 	// test render
// 	for( int pointIndex = 0; pointIndex < m_tempPoints.size(); pointIndex++ ) {
// 		g_theRenderer->DrawCircle( Vec3( m_tempPoints[pointIndex] ), 0.5f, 0.5f, Rgba8::RED );
// 	}
// 
// 	PolygonCollider2D* testPolyCol =  (PolygonCollider2D*)m_gameObjects[0]->m_rb->GetCollider();
// 	
// 	for( int pointIndex = 0; pointIndex < m_testPoints.size(); pointIndex++ ) {
// 		const Vec2& point = m_testPoints[pointIndex];
// 		if( testPolyCol->Contains( point ) ){
// 			g_theRenderer->DrawCircle( Vec3( point ), 0.5f, 0.5f, Rgba8::GREEN );
// 		}
// 		else {
// 			g_theRenderer->DrawCircle( Vec3( point ), 0.5f, 0.5f, Rgba8::YELLOW );
// 		}
// 		Vec2 closestPoint = testPolyCol->GetClosestPoint( point );
// 		g_theRenderer->DrawLine( closestPoint, point, 0.2f, Rgba8::WHITE );
// 		g_theRenderer->DrawCircle( Vec3( closestPoint ), 0.5f, 0.5f, Rgba8::WHITE );
// 	}
	g_theRenderer->BindTexture( nullptr );
	if( m_isDrawMode ) {
		RenderDrawingPolygon();
	}

}

void Game::RenderUI() const
{
}

void Game::GenerateTempPoints()
{
	for( int i = 0; i < 30; i++ ) {
		Vec2 tempPoint;
		tempPoint.x = m_rng->RollRandomFloatInRange( 15, 30 );
		tempPoint.y = m_rng->RollRandomFloatInRange( 15, 30	);
		m_tempPoints.push_back( tempPoint );
	}
}

void Game::GenerateTestPoints()
{
	for( int i = 0; i < 5; i++ ) {
		Vec2 tempPoint;
		tempPoint.y = m_rng->RollRandomFloatInRange( 10, 40 );
		tempPoint.x = m_rng->RollRandomFloatInRange( 10, 40 );
		m_testPoints.push_back( tempPoint );
	}
}

void Game::AddPointToDrawPolygon( Vec2 point )
{
	// Check if point valid
	// add point to list
	if( !IsPointEnoughForDrawPolygon() ){
		m_drawPoints.push_back( point );
	}
	else {
		if( IsDrawedPolygonConvex() ) {
			m_drawPoints.push_back( point );
		}

	}
}

void Game::RenderDrawingPolygon() const
{
	for( int pointIndex = 1; pointIndex < m_drawPoints.size(); pointIndex++ ) {
		Vec2 current = m_drawPoints[pointIndex -1];
		Vec2 next = m_drawPoints[pointIndex];
		g_theRenderer->DrawLine( current, next, 0.2f, Rgba8::BLUE );
	}



	Vec2 pos = m_mousePos;
	Vec2 last = m_drawPoints.back();
	if ( !IsDrawedPolygonConvex() ){
		g_theRenderer->DrawLine( last, pos, 0.2f, Rgba8::RED );
	}
	else {
		g_theRenderer->DrawLine( last, pos, 0.2f, Rgba8::BLUE );
	}
}

bool Game::SelectGameObject()
{
	if( m_gameObjects.size() == 0 ) { return false; }

	for( int objIndex = (int)m_gameObjects.size() - 1; objIndex >= 0; objIndex-- ) {
		if( m_gameObjects[objIndex] == nullptr ) { continue; }
		if( m_gameObjects[objIndex]->m_isMouseIn ) {
			m_selectedObj = m_gameObjects[objIndex];
			m_selectedObj->m_isSelected = true;
			return true;
		}
	}
	return false;
}

void Game::LoadAssets()
{
	
}

