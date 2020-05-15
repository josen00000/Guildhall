#include <windows.h>
#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Platform/Window.hpp"

extern App*				g_theApp;
extern BitmapFont*		g_squirrelFont;	
extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInputSystem;
extern DevConsole*		g_theConsole;
extern Physics2D*		g_thePhysics;
extern Window*			g_theWindow;

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
	
	//
	//Vec2 polyPoints[5] ={ Vec2( 10, 10 ), Vec2( 20, 10 ), Vec2( 20, 20 ), Vec2( 15, 15 ), Vec2( 10, 20 )};
	GenerateTempPoints();
	GenerateTestPoints();
	//Polygon2 tempPoly = Polygon2::MakeConvexFromPointCloud( m_tempPoints );
	//GameObject* polyTest = new GameObject( tempPoly );
	//m_gameObjects.push_back( polyTest );

	// test debug render
	//DebugAddScreenText( Vec4( 0.f, 1.f, 20.f, -20.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, -1.f, "teststring" );
	DebugAddWorldPoint( Vec3( 50.f, 50.f, 0.f ), 50.f, Rgba8::RED, 1.f, DEBUG_RENDER_ALWAYS );
	//DebugAddWorldPoint( Vec3( 0.f, 0.f, -50.f ), Rgba8::RED, -1.f, DEBUG_RENDER_ALWAYS );

	// test contact
	CreateContactTestObjects();

	// CreateFloor
	CreateWorldFloor();

}

void Game::Shutdown()
{
	delete m_rng;
	delete m_gameCamera;
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
	m_gameCamera->SetClearMode( CLEAR_COLOR_BIT, Rgba8::RED, 0.0f, 0 );
	IsMouseOverObject();

	CleanDestroyedObjects();
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
	HWND tophWnd = (HWND)Window::GetTopWindowHandle();
	Vec2 clientPos = g_theInputSystem->GetNormalizedMousePosInClient( tophWnd );
	Vec2 mouseClientPos( clientPos.x, clientPos.y );
	m_mousePos = m_gameCamera->ClientToWorld( mouseClientPos, 1 );
	std::string mouseClientPosStr = std::string(" client pos = " + std::to_string( mouseClientPos.x ) + "  " + std::to_string( mouseClientPos.y ) );
	std::string mouseWorldPos = std::string(" world pos = " + std::to_string( m_mousePos.x) + "  " + std::to_string( m_mousePos.y ) );
	//g_theConsole->PrintString( Rgba8::RED, mouseClientPosStr );
	//g_theConsole->PrintString( Rgba8::BLUE, mouseWorldPos );

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
	std::string heightText = "height text :" + std::to_string( height );
	DebugAddScreenText( Vec4( 0.f, 1.f, 10.f, -10.f), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 0.1f, heightText );
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
	m_gameCamera->SetOrthoView( Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ), 0.9f );
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
		g_theConsole->SetIsOpen( true );
		g_theConsole->PrintString( Rgba8::RED, std::string( "isopen" ) );
		g_theConsole->PrintString( Rgba8::RED, std::string( "isopen" ) );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_C ) ) {
		g_theConsole->SetIsOpen( false );
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
		else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_4 ) ) {
			m_selectedObj->m_rb->EnableTrigger();
		}
		else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_5 ) ) {
			m_selectedObj->m_rb->DisableTrigger();
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_PLUS ) ) {
			if( m_selectedObj != nullptr ){
				m_selectedObj->UpdateBounciness( 0.01f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_MINUS ) ) {
			if( m_selectedObj != nullptr ){
				m_selectedObj->UpdateBounciness( -0.01f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_LEFT_BRACKET ) ){
			if( m_selectedObj != nullptr) {
				m_selectedObj->UpdateMass( -0.1f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_RIGHT_BRACKET ) ){
			if( m_selectedObj != nullptr ) {
				m_selectedObj->UpdateMass( 0.1f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_COMMA ) ){
			if( m_selectedObj != nullptr ){
				m_selectedObj->UpdateFriction( -0.01f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_PERIOD ) ) {
			if( m_selectedObj != nullptr ) {
				m_selectedObj->UpdateFriction( 0.01f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_SEMICOLON ) ) {
			if( m_selectedObj != nullptr ) {
				m_selectedObj->UpdateDrag( -0.01f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_BACKSLASH ) ) {
			if( m_selectedObj != nullptr ) {
				m_selectedObj->UpdateDrag( 0.01f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_R ) ) {
			if( m_selectedObj != nullptr ) {
				m_selectedObj->UpdateRotationRadians( 0.1f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_E ) ) {
			if( m_selectedObj != nullptr ) {
				m_selectedObj->UpdateRotationRadians( -0.1f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_T ) ) {
			if( m_selectedObj != nullptr ) {
				m_selectedObj->UpdateAngularVelocity( 0.01f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_G ) ) {
			if( m_selectedObj != nullptr ) {
				m_selectedObj->UpdateAngularVelocity( -0.01f );
			}
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_V ) ) {
			if( m_selectedObj != nullptr ) {
				m_selectedObj->ResetAngularVelocity();
			}
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
		if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_P ) ){
			static bool isPause = false;
			if( isPause ){
				g_thePhysics->PausePhysicsTime();
			}
			else{
				g_thePhysics->ResumePhysicsTime();
			}
			isPause = !isPause;
		}
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_8 ) ){
			double scale = g_thePhysics->GetTimeScale();
			scale /= 2;
			g_thePhysics->SetTimeScale( scale );
		}
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_9 ) ){
			double scale = g_thePhysics->GetTimeScale();
			scale *= 2;
			g_thePhysics->SetTimeScale( scale );
		}
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_0 ) ) {
			g_thePhysics->SetTimeScale( 1 );
			g_thePhysics->ResumePhysicsTime();
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
	//tempPoints.push_back( m_mousePos );
	if( tempPoints.size() < 3 ){ return false; }
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
	//g_theRenderer->BindShader( "Data/Shader/lit.hlsl" );
	g_theRenderer->SetDiffuseTexture( nullptr );
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		if( m_gameObjects[objIndex] == nullptr ){ continue; }
		m_gameObjects[objIndex]->Render();
	}
 	// test render
//  	for( int pointIndex = 0; pointIndex < m_tempPoints.size(); pointIndex++ ) {
//  		g_theRenderer->DrawCircle( Vec3( m_tempPoints[pointIndex] ), 0.5f, 0.5f, Rgba8::RED );
//  	}
 
 	//PolygonCollider2D* testPolyCol =  (PolygonCollider2D*)m_gameObjects[0]->m_rb->GetCollider();
 	
//  	for( int pointIndex = 0; pointIndex < m_testPoints.size(); pointIndex++ ) {
//  		const Vec2& point = m_testPoints[pointIndex];
//  		if( testPolyCol->Contains( point ) ){
//  			g_theRenderer->DrawCircle( Vec3( point ), 0.5f, 0.5f, Rgba8::GREEN );
//  		}
//  		else {
//  			g_theRenderer->DrawCircle( Vec3( point ), 0.5f, 0.5f, Rgba8::YELLOW );
//  		}
//  		Vec2 closestPoint = testPolyCol->GetClosestPoint( point );
//  		g_theRenderer->DrawLine( closestPoint, point, 0.2f, Rgba8::WHITE );
//  		g_theRenderer->DrawCircle( Vec3( closestPoint ), 0.5f, 0.5f, Rgba8::WHITE );
//  	}
	if( m_isDrawMode ) {
		RenderDrawingPolygon();
	}

}

void Game::RenderUI() const
{
	RenderGravity();
	RenderTime();
	RenderMouse();
	RenderToolTip();
}

void Game::RenderToolTip() const
{
	float textHeight = 1.5f;
	if( m_overObj != nullptr && m_overObj->m_rb != nullptr ){
		HWND hWnd = (HWND)g_theWindow->GetTopWindowHandle();
		Vec2 clientPos = g_theInputSystem->GetNormalizedMousePosInClient( hWnd );
		Vec2 UIMousePos = m_UICamera->GetCameraAsBox().GetPointAtUV( clientPos );
		AABB2 toolBox = AABB2( UIMousePos + Vec2( 20, 20 ), UIMousePos + Vec2( 80, 40 ));
		g_theRenderer->SetDiffuseTexture( nullptr );
		g_theRenderer->DrawAABB2D( toolBox, Rgba8::GRAY );
		g_theRenderer->SetDiffuseTexture( g_squirrelFont->GetTexture() );

		std::vector<Vertex_PCU> toolVertices;
		std::string mode;
		switch( m_overObj->m_rb->GetSimulationMode() )
		{
			case RIGIDBODY_STATIC:{
				mode = "Static";
				break;
			}
			case RIGIDBODY_DYNAMIC: {
				mode = "Dynamic";
				break;
			}
			case RIGIDBODY_KINEMATIC:{
				mode = "Kinematic";
				break;
			}
			default: 
				break;
		}

		float rotationDegree = ConvertRadiansToDegrees( m_overObj->m_rb->GetRotationInRadians() );

		std::string toolMode			= "The simulation mode : " +  mode;
		std::string toolMass			= "Mass : " + std::to_string( m_overObj->m_rb->GetMass() );
		std::string toolVelocity		= "Velocity : " + std::to_string( m_overObj->m_rb->GetVerletVelocity().x ) + ", " + std::to_string( m_overObj->m_rb->GetVerletVelocity().y );
		std::string toolVerletVelocity	= "Verlet Velocity : " + std::to_string( m_overObj->m_rb->GetVelocity().x ) + ", " + std::to_string( m_overObj->m_rb->GetVelocity().y );
		std::string toolBounce			= "Restitution : " + std::to_string( m_overObj->m_rb->GetCollider()->GetBounciness() );
		std::string toolFric			= "Friction : " + std::to_string( m_overObj->m_rb->GetCollider()->GetFriction() );
		std::string toolDrag			= "Drag : " + std::to_string( m_overObj->m_rb->GetDrag() );
		std::string toolMoment			= " Moment : " + std::to_string( m_overObj->m_rb->GetMoment() );
		std::string toolRotation		= " Rotation : " + std::to_string( rotationDegree );
		std::string toolAngularVel		= " Angular Velocity : " + std::to_string( m_overObj->m_rb->GetAngularVelocity() );
		
		float height = toolBox.maxs.y - toolBox.mins.y;
		g_squirrelFont->AddVertsForTextInBox2D( toolVertices, toolBox, textHeight, toolMode, Rgba8::RED, 1.f, Vec2::ZERO );
		g_squirrelFont->AddVertsForTextInBox2D( toolVertices, toolBox, textHeight, toolMass, Rgba8::RED, 1.f, Vec2::ZERO + Vec2( 0, textHeight * 1 / height  ));
		g_squirrelFont->AddVertsForTextInBox2D( toolVertices, toolBox, textHeight, toolVelocity, Rgba8::RED, 1.f, Vec2::ZERO + Vec2( 0, textHeight * 2 /height) );
		g_squirrelFont->AddVertsForTextInBox2D( toolVertices, toolBox, textHeight, toolVerletVelocity, Rgba8::RED, 1.f, Vec2::ZERO + Vec2( 0, textHeight * 3/height ) );
		g_squirrelFont->AddVertsForTextInBox2D( toolVertices, toolBox, textHeight, toolBounce, Rgba8::RED, 1.f, Vec2::ZERO+ Vec2( 0, textHeight * 4/height ) );
		g_squirrelFont->AddVertsForTextInBox2D( toolVertices, toolBox, textHeight, toolFric, Rgba8::RED, 1.f, Vec2::ZERO+ Vec2( 0, textHeight * 5/height ) );
		g_squirrelFont->AddVertsForTextInBox2D( toolVertices, toolBox, textHeight, toolDrag, Rgba8::RED, 1.f, Vec2::ZERO+ Vec2( 0, textHeight * 6/height ) );
		g_squirrelFont->AddVertsForTextInBox2D( toolVertices, toolBox, textHeight, toolMoment, Rgba8::RED, 1.f, Vec2::ZERO+ Vec2( 0, textHeight * 7/height ) );
		g_squirrelFont->AddVertsForTextInBox2D( toolVertices, toolBox, textHeight, toolRotation, Rgba8::RED, 1.f, Vec2::ZERO+ Vec2( 0, textHeight * 8/height ) );
		g_squirrelFont->AddVertsForTextInBox2D( toolVertices, toolBox, textHeight, toolAngularVel, Rgba8::RED, 1.f, Vec2::ZERO+ Vec2( 0, textHeight * 9/height ) );
		g_theRenderer->DrawVertexVector( toolVertices );
	}
}

void Game::RenderGravity() const
{
	g_theRenderer->SetDiffuseTexture( g_squirrelFont->GetTexture() );
	std::vector<Vertex_PCU> gravityVertices;
	std::string gravity = "The Gravity is: " + std::to_string( g_thePhysics->m_gravityAccel.y );
	g_squirrelFont->AddVertsForTextInBox2D( gravityVertices, m_UICamera->GetCameraAsBox(), 3.f, gravity, Rgba8::RED, 1.f, Vec2::ZERO );
	g_theRenderer->DrawVertexVector( gravityVertices );
}

void Game::RenderMouse() const
{
	g_theRenderer->SetDiffuseTexture( g_squirrelFont->GetTexture() );
	std::vector<Vertex_PCU> mouseVertices;
	std::string gravity = "The mousePos is: " + std::to_string( m_mousePos.x ) + " , " + std::to_string( m_mousePos.y );
	g_squirrelFont->AddVertsForTextInBox2D( mouseVertices, m_UICamera->GetCameraAsBox(), 3.f, gravity, Rgba8::RED, 1.f, Vec2( 0.f, ( 9 / m_UICamera->GetCameraHeight() ) ) );
	g_theRenderer->DrawVertexVector( mouseVertices );

}

void Game::RenderTime() const
{
	g_theRenderer->SetDiffuseTexture( g_squirrelFont->GetTexture() );
	std::vector<Vertex_PCU> timeVertices;
	std::string time;
	if( g_thePhysics->IsClockPause() ){
		time = "Time Scale: " + std::to_string( g_thePhysics->GetTimeScale() ) + ".    Pause state: True.";
	}
	else{
		time = "Time Scale: " + std::to_string( g_thePhysics->GetTimeScale() ) + ".    Pause state: False.";
	}
	g_squirrelFont->AddVertsForTextInBox2D( timeVertices, m_UICamera->GetCameraAsBox(), 3.f, time, Rgba8::RED, 1.f, Vec2( 0.f, ( 6 / m_UICamera->GetCameraHeight() ) ) );
	g_theRenderer->DrawVertexVector( timeVertices );
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

void Game::CreateContactTestObjects()
{
	std::vector<Vec2> cubePoints;
	cubePoints.push_back( Vec2( 10.f, 10.f ) );
	cubePoints.push_back( Vec2( 30.f, 10.f ) );
	cubePoints.push_back( Vec2( 30.f, 30.f ) );
	cubePoints.push_back( Vec2( 10.f, 30.f ) );

	std::vector<Vec2> triPoints;
	triPoints.push_back( Vec2( 40.f, 40.f ) );
	triPoints.push_back( Vec2( 60.f, 40.f ) );
	triPoints.push_back( Vec2( 50.f, 50.f ) );

	GameObject* testCubeObject = new GameObject( cubePoints );
	GameObject* testTriObject = new GameObject( triPoints );
	testTriObject->m_rb->SetLayer( 1 );
	g_thePhysics->EnableLayerInteraction( 0, 1 );

	testCubeObject->m_rb->m_userProperties.SetValue("GameObject", *testTriObject );
	GameObject iniObj = GameObject( Vec2::ZERO, 0.f );
	static GameObject testObj = testCubeObject->m_rb->m_userProperties.GetValue("GameObject",  iniObj );
	m_gameObjects.push_back( testCubeObject );
	m_gameObjects.push_back( testTriObject );
}

void Game::CreateWorldFloor()
{
	std::vector<Vec2> floorPoints;
	floorPoints.push_back( Vec2( 0.f, -0.5f ) );
	floorPoints.push_back( Vec2( 160.f, -0.5f ) );
	floorPoints.push_back( Vec2( 160.f, 0.5f ) );
	floorPoints.push_back( Vec2( 0.f, 0.5f ) );
	GameObject* floorObject = new GameObject( floorPoints );
	m_gameObjects.push_back( floorObject );
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
	//tempGameObj->DisablePhysics();
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

void Game::CleanDestroyedObjects()
{
	for( int i = 0; i < m_gameObjects.size(); i++ ) {
		if( m_gameObjects[i] == nullptr ){ continue; }
		if( m_gameObjects[i]->IsDestroyed() ) {
			DeleteGameObject( m_gameObjects[i] );
		}
	}
}

void Game::OnTriggerEnter( Collision2D colls )
{
	DebugAddScreenText( Vec4( 0.5f, 0.4f, 20.f, -20.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 1.f, "Trigger Enter" );
}

void Game::OnTriggerStay( Collision2D colls )
{
	DebugAddScreenText( Vec4( 0.5f, 0.5f, 20.f, -20.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 0.1f, "Trigger Stay" );
}

void Game::OnTriggerLeave( Collision2D colls )
{
	DebugAddScreenText( Vec4( 0.5f, 0.6f, 20.f, -20.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 1.f, "Trigger Leave" );
}

void Game::OnCollliderEnter( Collision2D colls )
{
	DebugAddScreenText( Vec4( 0.5f, 0.7f, 20.f, -20.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 1.f, "Collider Enter" );
}

void Game::OnCollliderStay( Collision2D colls )
{
	DebugAddScreenText( Vec4( 0.5f, 0.8f, 20.f, -20.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 0.1f, "Collider stay" );
}

void Game::OnCollliderLeave( Collision2D colls )
{
	DebugAddScreenText( Vec4( 0.5f, 0.9f, 20.f, -20.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 1.f, "Collider Leave" );
}

bool Game::IsMouseOverObject( )
{
	if( m_gameObjects.size() == 0 ) { return false; }

	for( int objIndex = (int)m_gameObjects.size() - 1; objIndex >= 0; objIndex-- ) {
		if( m_gameObjects[objIndex] == nullptr ) { continue; }
		if( m_gameObjects[objIndex]->m_isMouseIn ) {
			m_overObj = m_gameObjects[objIndex];
			return true;
		}
	}
	m_overObj = nullptr;
	return false;
}

void Game::LoadAssets()
{
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
}

