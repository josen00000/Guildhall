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
extern BitmapFont*		g_squirrelFont;	
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
	LoadAssets();
	//BitmapFont* 
	//g_theConsole = new DevConsole( testFont );
	//
	//Vec2 polyPoints[5] ={ Vec2( 10, 10 ), Vec2( 20, 10 ), Vec2( 20, 20 ), Vec2( 15, 15 ), Vec2( 10, 20 )};
	GenerateTempPoints();
	GenerateTestPoints();
	Polygon2 tempPoly = Polygon2::MakeConvexFromPointCloud( m_tempPoints );
	GameObject* polyTest = new GameObject( tempPoly );
	m_gameObjects.push_back( polyTest );
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
	UpdateMouse( deltaSeconds );
	UpdateCamera( deltaSeconds );
	UpdatePhysics( deltaSeconds );
	UpdateGameObjects( deltaSeconds );
	UpdateGameObjectsIntersect();
}

void Game::UpdatePhysics( float deltaSeconds )
{
	g_thePhysics->Update( deltaSeconds );
}

void Game::UpdateUI( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::UpdateMouse( float deltaSeconds)
{
	UpdateMouseWheel();
	UpdateMousePos();
	UpdateMouseVelocity( deltaSeconds );
	HandleMouseInput();
	HandleKeyboardInput();
}

void Game::UpdateMouseVelocity( float deltaSeconds )
{
	// calculate the mouse velocity with positions
	// if positions < 3 
	if( m_mousePositions.size() == 1 ) {
		return;
	}
	else {
		m_mouseVelocity = ( m_mousePositions.back() - m_mousePositions.front() ) / deltaSeconds;
		m_mousePositions.pop();
	}
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
			m_selectedObj->SetPosition( m_mousePos - m_selectOffset );
		}
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) || g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_DELETE ) ) {
			m_selectedObj->m_isSelected =  false;
			DeleteGameObject( m_selectedObj );
			m_selectedObj = nullptr;
		}
	}

	// Record mouse positions
	m_mousePositions.push( m_mousePos );
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
	UNUSED(deltaSeconds);
	float clampedScroll = ClampFloat( 0.f, 100.f, m_mouseScroll );
	float height = RangeMapFloat( 0.f, 100.f, 100.f, 50.f, clampedScroll );

	m_gameCamera->SetProjectionOrthographic( height );
}

void Game::UpdateCameraPos( float deltaSeconds )
{
	m_cameraMoveDirct = Vec2::ZERO;
	
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

	Vec2 cameraVelocity = m_cameraMoveDirct * m_cameraMoveSpeed;
	Vec2 cameraDisp = cameraVelocity * deltaSeconds;
	Vec2 currentCameraPos = m_gameCamera->GetPosition();
	m_gameCamera->SetPosition( currentCameraPos + cameraDisp );
}

void Game::SetCameraToOrigin()
{
	m_gameCamera->SetOrthoView( Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ) );
	g_theInputSystem->ResetMouseWheel();
}

void Game::HandleMouseInput()
{
	// button down
	if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_LEFT ) ){
		if( m_isDrawMode ){
			AddPointToDrawPolygon( m_mousePos );
		}
		else if( SelectGameObject() ) {
			m_isDragging = true;
			m_selectOffset = m_mousePos - m_selectedObj->GetPosition();
		}
	}
	if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_RIGHT ) ) {
		EndDrawPolygon();
	}

	// button up
	if( g_theInputSystem->WasMouseButtonJustReleased( MOUSE_BUTTON_LEFT ) ) {
		m_isDragging = false;
		if( m_selectedObj != nullptr ) {
			m_selectedObj->EnablePhysics();
			m_selectedObj->SetVelocity( m_mouseVelocity );
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
	if( m_isDragging ) {
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_1 ) ) {
			m_selectedObj->SetSimulateMode( RIGIDBODY_STATIC );
		}
		else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_2 ) ) {
			m_selectedObj->SetSimulateMode( RIGIDBODY_KINEMATIC );
		}
		else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_3 ) ) {
			m_selectedObj->SetSimulateMode( RIGIDBODY_DYNAMIC );
		}
	}
	else {
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_1) ){
			CreateDiscGameObject();
		}
		// draw input
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_2 ) ) {
			if( !m_isDrawMode ){
				BeginDrawPolygon();
			}
		}
		if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_PLUS ) ) {
			g_thePhysics->ModifyGravity( -1 );
		}
		if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_MINUS ) ) {
			g_thePhysics->ModifyGravity( 1 );
		}
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_O ) ) {
			SetCameraToOrigin();
		}
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
		//AddPointToDrawPolygon( m_drawPoints[0] );
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
	g_theRenderer->BindTexture( nullptr );
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		if( m_gameObjects[objIndex] == nullptr ){ continue; }
		m_gameObjects[objIndex]->Render();
	}
 	// test render
 	for( int pointIndex = 0; pointIndex < m_tempPoints.size(); pointIndex++ ) {
 		g_theRenderer->DrawCircle( Vec3( m_tempPoints[pointIndex] ), 0.5f, 0.5f, Rgba8::RED );
 	}
 
 	PolygonCollider2D* testPolyCol =  (PolygonCollider2D*)m_gameObjects[0]->m_rb->GetCollider();
 	
 	for( int pointIndex = 0; pointIndex < m_testPoints.size(); pointIndex++ ) {
 		const Vec2& point = m_testPoints[pointIndex];
 		if( testPolyCol->Contains( point ) ){
 			g_theRenderer->DrawCircle( Vec3( point ), 0.5f, 0.5f, Rgba8::GREEN );
 		}
 		else {
 			g_theRenderer->DrawCircle( Vec3( point ), 0.5f, 0.5f, Rgba8::YELLOW );
 		}
 		Vec2 closestPoint = testPolyCol->GetClosestPoint( point );
 		g_theRenderer->DrawLine( closestPoint, point, 0.2f, Rgba8::WHITE );
 		g_theRenderer->DrawCircle( Vec3( closestPoint ), 0.5f, 0.5f, Rgba8::WHITE );
 	}
	if( m_isDrawMode ) {
		RenderDrawingPolygon();
	}

}

void Game::RenderUI() const
{
	RenderGravity();
}

void Game::RenderGravity() const
{
	g_theRenderer->BindTexture( g_squirrelFont->GetTexture() );
	std::vector<Vertex_PCU> gravityVertices;
	std::string gravity = "The Gravity is: " + std::to_string( g_thePhysics->m_gravityAccel.y );
	g_squirrelFont->AddVertsForTextInBox2D( gravityVertices, m_UICamera->GetCameraBox(), 3.f, gravity, Rgba8::RED, 1.f, Vec2::ZERO );
	g_theRenderer->DrawVertexVector( gravityVertices );
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
			m_selectedObj->DisablePhysics();
			return true;
		}
	}
	return false;
}

void Game::CreateDiscGameObject()
{
	float tempRadius = m_rng->RollRandomFloatInRange( 5, 15 );
	GameObject* tempGameObj = new GameObject( m_mousePos, tempRadius );
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		if( m_gameObjects[objIndex] == nullptr ) {
			m_gameObjects[objIndex] = tempGameObj;
			return;
		}
	}
	m_gameObjects.push_back( tempGameObj );
}

void Game::UpdateGameObjects( float deltaSeconds )
{
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		GameObject* obj = m_gameObjects[objIndex];
		if( obj == nullptr ){ continue; }

		obj->Update( deltaSeconds );
	}
}

void Game::UpdateGameObjectsIntersect()
{
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		GameObject* obj = m_gameObjects[objIndex];
		if( obj == nullptr ) { continue; }

		obj->SetIntersect( false );

		for( int objIndex1 = 0; objIndex1 < m_gameObjects.size(); objIndex1++ ) {
			GameObject* obj1 = m_gameObjects[objIndex1];
			if( obj1 == nullptr || objIndex1 == objIndex ){ continue; }
			
			obj1->CheckIntersectWith( obj );
		}
	}
}

void Game::DeleteGameObject( GameObject* obj )
{
	if( obj == nullptr ) { return; }

	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		if( m_gameObjects[objIndex] == obj ) {
			delete m_gameObjects[objIndex];
			m_gameObjects[objIndex] = nullptr;
		}
	}
}

void Game::LoadAssets()
{
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
}

