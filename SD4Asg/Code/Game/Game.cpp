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
	GenerateGameObjects();
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_gameCamera;
}

void Game::RunFrame( float deltaSeconds )
{
	CheckIfExit();
	HandleInput();
	Update( deltaSeconds );
	UpdateMouse( deltaSeconds );
	UpdateDraggedObject();
}

void Game::Update( float deltaSeconds )
{
	for( GameObject* obj : m_objs )
	{
		obj->UpdateObject( deltaSeconds );
	}
}


void Game::UpdateMouse( float deltaSeconds )
{
	HWND tophWnd = (HWND)g_theWindow->GetHandle();
	Vec2 clientPos = g_theInputSystem->GetNormalizedMousePosInClient( tophWnd );
	m_mousePos = m_gameCamera->ClientToWorld( clientPos, 1 );
}

void Game::UpdateDraggedObject()
{
	if( m_isDragging && m_selectedObj ) {
		std::vector<Vec2> movedPolyPoints;
		for( Vec2 offset : m_draggedOffsets ) {
			movedPolyPoints.push_back( offset + m_mousePos );
		}
		ConvexPoly2 poly = ConvexPoly2( movedPolyPoints );
		m_selectedObj->SetConvexPoly( poly );
	}
	
}

void Game::StartDrag()
{
	m_draggedOffsets.clear();
	ConvexPoly2 objPoly = m_selectedObj->GetPoly();
	for( Vec2 point : objPoly.m_points ) {
		m_draggedOffsets.push_back( point - m_mousePos );
	}
}

void Game::GenerateGameObjects()
{
	int objNum = m_rng->RollRandomIntLessThan( 50 );
	//int objNum = 1;
	AABB2 worldCameraBox = m_gameCamera->GetCameraAsBox();
	for( int i = 0; i < objNum; i++ ) {
		RandomGenerateNewObject( worldCameraBox );
	}
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) ){
		
		g_theApp->HandleQuitRequested();
	}
}


void Game::HandleInput()
{
	HandleKeyboardInput();
	HandleMouseInput();
}

void Game::HandleKeyboardInput()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_COMMA ) ) {
		if( m_objs.size() > 2 ) {
			HalveObjects();			
		}
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_PERIOD ) ) {
		if( m_objs.size() != 0 ) {
			DoubleObjects();
		}
	}
	
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_Q ) ) {
		if( m_selectedObj ) {
			RotateObjectAtPoint( m_selectedObj, ROTATE_LEFT, m_mousePos );
		}
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_E ) ) {
		if( m_selectedObj ) {
			RotateObjectAtPoint( m_selectedObj, ROTATE_RIGHT, m_mousePos );
		}
	}
	else {
		if( m_selectedObj ) {
			m_selectedObj->SetIsRotating( false );
		}
	}

	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_L ) ) {
		if( m_selectedObj ) {
			m_selectedObj->ScaleObjectWithPoint( 2.f, m_mousePos );
		}
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_K ) ) {
		if( m_selectedObj ) {
			m_selectedObj->ScaleObjectWithPoint( 0.5f, m_mousePos );
		}
	}
}


void Game::HandleMouseInput()
{
	if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_LEFT ) && m_selectedObj ) {
		m_isDragging = true;
		StartDrag();

	}
	if( !g_theInputSystem->IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {
		m_isDragging = false;
	}
}

void Game::RandomGenerateNewObject( AABB2 bounds )
{
	Vec2 objPos = m_rng->RollPositionInBox( bounds );
	GameObject* tempObj = new GameObject( objPos );
	m_objs.push_back( tempObj );
}

void Game::HalveObjects()
{
	int currentObjNum = m_objs.size();
	currentObjNum /= 2;
	for( int i = currentObjNum; i < m_objs.size(); i++ ) {
		delete m_objs[i];
	}
	m_objs.resize( currentObjNum );
}

void Game::DoubleObjects()
{
	int targetObjNum = m_objs.size() * 2;
	m_objs.reserve( targetObjNum );
	AABB2 worldCameraBox = m_gameCamera->GetCameraAsBox();
	while( m_objs.size() < targetObjNum ) {
		RandomGenerateNewObject( worldCameraBox );
	}
}

void Game::RotateObjectAtPoint( GameObject* obj, RotateDirection dirt, Vec2 point )
{
	obj->SetIsRotating( true );
	obj->SetRotateDirt( dirt );
	obj->SetCenter( point );
}

void Game::SetSelectedObject( GameObject* obj )
{
	m_selectedObj = obj;
}

void Game::Render() const
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	for( int i = 0; i < m_objs.size(); i++ ) {
		m_objs[i]->RenderObject();
	}
	g_theRenderer->DrawCircle( Vec3( m_mousePos), 0.2f, 0.2f, Rgba8::RED );
	RenderUI();
}

void Game::RenderUI() const
{
	Strings debugInfo;
	debugInfo.push_back( Stringf( "Objects Num: %i", m_objs.size() ) );
	DebugAddScreenLeftAlignStrings( 0.f, 85.f, Rgba8::WHITE, debugInfo );
}



void Game::LoadAssets()
{
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
}

