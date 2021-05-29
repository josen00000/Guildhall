#include <windows.h>
#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/ShapeUtils.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"

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
	GenerateInvisibleRaycast();
	ConstructSPTree();
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_gameCamera;
}

void Game::RunFrame( float deltaSeconds )
{
	float beforeTime = (float)GetCurrentTimeSeconds();
	CheckIfExit();
	HandleInput();
	if( m_isPaused ) {
		deltaSeconds = 0.f ;
	}
	else {
		UpdateMouse( );
	}
	Update( deltaSeconds );
	UpdateDraggedObject();


	// append points
	m_points.clear();
	m_indexes.clear();
	m_points.reserve( 15000 );
	m_indexes.reserve( 15000 );
	for( int i = 0; i < m_objs.size(); i++ ) {
		m_objs[i]->AppendPointsToVector( m_points, m_indexes );
	}
	float afterTime = (float)GetCurrentTimeSeconds();
	m_debugTime = afterTime - beforeTime;
}

void Game::Update( float deltaSeconds )
{
	m_deltaSeconds = deltaSeconds;
	for( GameObject* obj : m_objs )
	{
		obj->UpdateObject( deltaSeconds );
	}
	UpdateVisibleRaycast();
	UpdateInvisibleRaycast();
}


void Game::UpdateMouse( )
{
	HWND tophWnd = (HWND)g_theWindow->GetHandle();
	Vec2 clientPos = g_theInputSystem->GetNormalizedMousePosInClient( tophWnd );
	m_mousePos = m_gameCamera->ClientToWorld( clientPos, 1 );
}

void Game::RestartGame()
{
	m_objs.clear();
	m_invisibleRaycast.clear();
	GenerateGameObjects();
	GenerateInvisibleRaycast();
	ConstructSPTree();
}

void Game::RenderObjects() const
{
	// sol1 
	for( int i = 0; i < m_objs.size(); i++ ) {
		m_objs[i]->RenderObject();
	}
	// sol2
	g_theRenderer->DrawIndexedVertexVector( m_points, m_indexes );
	//g_theRenderer->DrawVertexArray()
}

void Game::GenerateInvisibleRaycast()
{
	AABB2 worldCameraBox = m_gameCamera->GetCameraAsBox();
	for( int i = 0; i < 1024; i++ ) {
		RaycastTest tempTest;
		tempTest.startPos =  m_rng->RollPositionInBox( worldCameraBox );
		float degrees = m_rng->RollRandomFloatInRange( 0.f, 360.f );
		Vec2 fwdDirt = Vec2::ONE_ZERO;
		fwdDirt.SetAngleDegrees( degrees );
		tempTest.fwdDirt = fwdDirt;
		tempTest.dist = m_rng->RollRandomFloatInRange( 0.f, 100.f );
		m_invisibleRaycast.push_back( tempTest );
	}
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

void Game::UpdateVisibleRaycast( )
{
	if( !m_isRaycast ){ return; }
	std::vector<GameObject*> possibleObjs;
	std::vector<ConvexPoly2> polygons;
	for( int i = 0; i < m_invisibleRaycast.size(); i++ ) {
		possibleObjs = GetPossibleObjsWithImplicitQuadTree( m_invisibleRaycast[i].startPos, m_invisibleRaycast[i].fwdDirt, m_invisibleRaycast[i].dist );
	}
	for( int i = 0; i < possibleObjs.size(); i++ ) {
		polygons.push_back( possibleObjs[i]->GetPoly() );
	}

	
	// temp for testing
	Vec2 fwdDirt = m_mousePos - m_testRaycastStartPoint;
	float dist = GetDistance2D( m_mousePos, m_testRaycastStartPoint );
	fwdDirt.Normalize();
	m_testRaycastResult = Raycast2DWithConvexPolygons( polygons, m_testRaycastStartPoint, fwdDirt, dist );
}

void Game::UpdateInvisibleRaycast()
{
	std::vector<ConvexPoly2> polygons;
	polygons.reserve( m_objs.size() );
	switch( m_strategy )
	{
	case NO_INVISIBLE_RAYCAST:
		return;
	case NO_BROAD_CHECK:
		for( int i = 0; i < m_objs.size(); i++ ) {
			polygons.push_back( m_objs[i]->GetPoly() );
		}
		hitNum1 = 0;
		for( int i = 0; i < m_invisibleRaycast.size(); i++ ) {
			m_invisibleRaycast[i].result = Raycast2DWithConvexPolygons( polygons, m_invisibleRaycast[i].startPos, m_invisibleRaycast[i].fwdDirt, m_invisibleRaycast[i].dist );
			if( m_invisibleRaycast[i].result.m_didImpact ) {
				hitNum1++;
			}
		}
		
		break;
	case IMPLICIT_QUAD_TREE:
		std::vector<GameObject*> possibleObjs;
		for( int i = 0; i < m_invisibleRaycast.size(); i++ ) {
			possibleObjs = GetPossibleObjsWithImplicitQuadTree( m_invisibleRaycast[i].startPos, m_invisibleRaycast[i].fwdDirt, m_invisibleRaycast[i].dist );
		}
		for( int i = 0; i < possibleObjs.size(); i++ ) {
			polygons.push_back( possibleObjs[i]->GetPoly() );
		}
		hitNum2 = 0;
		for( int i = 0; i < m_invisibleRaycast.size(); i++ ) {
			m_invisibleRaycast[i].result = Raycast2DWithConvexPolygons( polygons, m_invisibleRaycast[i].startPos, m_invisibleRaycast[i].fwdDirt, m_invisibleRaycast[i].dist );
			if( m_invisibleRaycast[i].result.m_didImpact ) {
				hitNum2++;
			}
		}
		break;
	}


}

void Game::StartSingleRaycast()
{
	m_isRaycast = true;
	m_testRaycastStartPoint = m_mousePos;
}

void Game::EndSingleRaycast()
{
	m_isRaycast = false;
}

void Game::RenderRaycast() const
{
	if( !m_isRaycast ){ return; }
	Vec2 debugFwdDirt = m_mousePos - m_testRaycastStartPoint;
	debugFwdDirt.Normalize();
	g_theRenderer->DrawCircle( m_testRaycastStartPoint, 0.1f, 0.1f, Rgba8::RED );
	g_theRenderer->DrawLine( m_testRaycastStartPoint, m_mousePos, 0.1f, Rgba8::YELLOW );
	if( m_testRaycastResult.m_didImpact ) {
		g_theRenderer->DrawCircle( m_testRaycastResult.m_impactPos, 0.3f, 0.3f, Rgba8::BLACK );
	}
	if( m_isDebug ) {
		DebugRenderRaycast();
	}
}

void Game::DebugRenderRaycast() const
{
	// 	// debug draw
	// 	float debugRadius = 0.2f;
	// 	float debugThick = 0.1f;
	// 	LineSegment2 debugLine( m_testRaycastStartPoint, m_mousePos );
	// 	Rgba8 debugColor = Rgba8::BLACK;
	// 	ConvexPoly2 debugPoly = m_objs[0]->GetPoly();
	// 	ConvexHull2 debugHull = ConvexHull2::MakeConvexHullFromConvexPoly( debugPoly );
	// 	Vec2 nearestPoint = debugLine.GetNearestPoint( debugPoly.GetCenter() );
	// 	Vec2 debugCenter = debugPoly.GetCenter();
	// 	float radius = debugPoly.GetBounderDiscRadius();
	// 	float dist = GetDistance2D( nearestPoint, debugCenter );
	// 	g_theRenderer->DrawCircle( debugCenter, debugPoly.GetBounderDiscRadius(), 0.1f, debugColor );
	// 	g_theRenderer->DrawCircle( nearestPoint, 0.3f, 0.2f, debugColor );
	// 	g_theRenderer->DrawCircle( debugCenter, 0.3f, 0.2f, debugColor );
	// 	g_theRenderer->DrawConvexHull( debugHull, Rgba8::GREEN );
	// 	g_theRenderer->DrawCircle( m_debugIntersectPoint, 0.5f, 0.3f, Rgba8::BLUE );
	// 
	// 	//
	// 
	// 	for( Plane2 plane : debugHull.m_planes ) {
	// 		Vec2 tempIntersectPoint = plane.GetIntersectPointWithRaycast( m_testRaycastStartPoint, debugFwdDirt, 30.f );
	// 		g_theRenderer->DrawCircle( tempIntersectPoint, 0.5f, 0.3f, Rgba8::CYAN );
	// 	}
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
	int objNum = 8; 
	AABB2 worldCameraBox = m_gameCamera->GetCameraAsBox();
	for( int i = 0; i < objNum; i++ ) {
		RandomGenerateNewObject( worldCameraBox );
	}
}

void Game::ConstructSPTree()
{
	AABB2 worldCamerabox = m_gameCamera->GetCameraAsBox();
	Plane2 bottom	= Plane2( Vec2( 0.f, -1.f ), worldCamerabox.mins );
	Plane2 top		= Plane2( Vec2( 0.f, 1.f ), worldCamerabox.maxs );
	Plane2 left		= Plane2( Vec2( -1.f, 0.f ), worldCamerabox.mins );
	Plane2 right	= Plane2( Vec2( 1.f, 0.f ), worldCamerabox.maxs );
	m_root = new SymmetricQuadTree<GameObject*>();
	 ConstructSPQuadTreeWithFourPlanes( m_root, bottom, top, left, right, m_objs, 0 );
}

void Game::ConstructSPQuadTreeWithFourPlanes( SymmetricQuadTree<GameObject*>* node, Plane2 bottom, Plane2 top, Plane2 left, Plane2 right, std::vector<GameObject*> content, int depth )
{
	depth++;
	std::vector<Plane2> boundPlanes;
	boundPlanes.push_back( bottom );
	boundPlanes.push_back( top );
	boundPlanes.push_back( left );
	boundPlanes.push_back( right );
	node->bound = ConvexHull2( boundPlanes );
	float midBTDist = ( bottom.m_dist + top.m_dist ) / 2.f;
	float midLRDist = ( left.m_dist + right.m_dist ) / 2.f;
	Plane2 midBottom	= Plane2( bottom.m_normal, bottom.m_dist - midBTDist );
	Plane2 midTop		= Plane2( top.m_normal, top.m_dist - midBTDist );
	Plane2 midLeft		= Plane2( left.m_normal, left.m_dist - midLRDist );
	Plane2 midRight		= Plane2( right.m_normal, right.m_dist - midLRDist );


	if( depth >= 2 ) {
		node->objs = content;
		node->children[0] = nullptr;
		node->children[1] = nullptr;
		node->children[2] = nullptr;
		node->children[3] = nullptr;
		return;
	}
	else {
		std::vector<GameObject*> leftBotContent;
		std::vector<GameObject*> rightBotContent;
		std::vector<GameObject*> leftTopContent;
		std::vector<GameObject*> rightTopContent;
		std::vector<Plane2> leftBotPlanes;
		std::vector<Plane2> rightBotPlanes;
		std::vector<Plane2> leftTopPlanes;
		std::vector<Plane2> rightTopPlanes;
		
		// left bottom sub bound planes
		leftBotPlanes.push_back( left );
		leftBotPlanes.push_back( bottom );
		leftBotPlanes.push_back( midTop );
		leftBotPlanes.push_back( midRight );

		// right bottom sub bound planes
		rightBotPlanes.push_back( right );
		rightBotPlanes.push_back( bottom );
		rightBotPlanes.push_back( midTop );
		rightBotPlanes.push_back( midLeft );


		// left top sub bound planes
		leftTopPlanes.push_back( left );
		leftTopPlanes.push_back( top );
		leftTopPlanes.push_back( midBottom );
		leftTopPlanes.push_back( midRight );

		// right top sub bound planes
		leftTopPlanes.push_back( right );
		leftTopPlanes.push_back( top );
		leftTopPlanes.push_back( midBottom );
		leftTopPlanes.push_back( midLeft );

		ConvexHull2 leftTopHull = ConvexHull2( leftTopPlanes );

		for( int i = 0; i < content.size(); i++ ) {
			GameObject* tempObj = content[i];
			if( tempObj->IsObjectInConvexHull( leftTopPlanes ) ) {
				leftTopContent.push_back( tempObj );
			}
			if( tempObj->IsObjectInConvexHull( leftBotPlanes ) ) {
				leftTopContent.push_back( tempObj );
			}
			if( tempObj->IsObjectInConvexHull( rightBotPlanes ) ) {
				leftTopContent.push_back( tempObj );
			}
			if( tempObj->IsObjectInConvexHull( rightTopPlanes ) ) {
				leftTopContent.push_back( tempObj );
			}
		}
		SymmetricQuadTree<GameObject*>* leftBotSubNode = new SymmetricQuadTree<GameObject *>();
		SymmetricQuadTree<GameObject*>* leftTopSubNode = new SymmetricQuadTree<GameObject *>();
		SymmetricQuadTree<GameObject*>* RightBotSubNode = new SymmetricQuadTree<GameObject *>();
		SymmetricQuadTree<GameObject*>* RightTopSubNode = new SymmetricQuadTree<GameObject *>();

		node->children[0] = leftBotSubNode;
		node->children[1] = leftTopSubNode;
		node->children[2] = RightBotSubNode;
		node->children[3] = RightTopSubNode;

		ConstructSPQuadTreeWithFourPlanes( leftBotSubNode, bottom, midTop, left, midRight, leftBotContent, depth );
		ConstructSPQuadTreeWithFourPlanes( leftTopSubNode, midBottom, top, left, midRight, leftTopContent, depth );
		ConstructSPQuadTreeWithFourPlanes( RightBotSubNode, bottom, midTop, midLeft, right, rightBotContent, depth );
		ConstructSPQuadTreeWithFourPlanes( RightTopSubNode, midBottom, top, midLeft, right, rightBotContent, depth );
	}
}

void Game::DebugRenderSPTree( SymmetricQuadTree<GameObject*>* node ) const
{
	std::vector<SymmetricQuadTree<GameObject*>*> result;
	std::vector<SymmetricQuadTree<GameObject*>*> intersectResult;
	node->GetAllLeavesNode( result );
	for( int i = 0; i < result.size(); i++ ) {
		ConvexHull2 hull = result[i]->GetConvexHull();
		g_theRenderer->DrawConvexHull( hull, Rgba8::BLACK );
	}
	if( m_isRaycast ) {
		Vec2 fwdDirt = m_mousePos - m_testRaycastStartPoint;
		float dist = GetDistance2D( m_mousePos, m_testRaycastStartPoint );
		fwdDirt.Normalize();
		node->GetAllIntersectLeavesNode( intersectResult, m_testRaycastStartPoint, fwdDirt, dist );
		for( int i = 0; i < intersectResult.size(); i++ ) {
			ConvexHull2 hull = intersectResult[i]->GetConvexHull();
			ConvexPoly2 poly = ConvexPoly2::MakeConvexPolyFromConvexHull( hull );
			g_theRenderer->DrawConvexPoly2D( poly, Rgba8( 255, 0, 0, 30 ) );
		}
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
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F1 ) ) {
		m_isDebug = !m_isDebug;
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F8 ) ) {
		RestartGame();
	}

	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_1 ) ) {
		m_strategy = NO_INVISIBLE_RAYCAST;
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_2 ) ) {
		m_strategy = NO_BROAD_CHECK;
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_3 ) ) {
		m_strategy = IMPLICIT_QUAD_TREE;
	}

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

	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P ) ) {
		m_isPaused = !m_isPaused;
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
	if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_LEFT ) && !m_isRaycast ) {
		StartSingleRaycast();
	}
	if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_RIGHT ) ) {
		EndSingleRaycast();
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
	int currentObjNum = (int)m_objs.size();
	currentObjNum /= 2;
	for( int i = currentObjNum; i < m_objs.size(); i++ ) {
		delete m_objs[i];
	}
	m_objs.resize( currentObjNum );
}

void Game::DoubleObjects()
{
	int targetObjNum = (int)m_objs.size() * 2;
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

std::vector<GameObject*> Game::GetPossibleObjsWithImplicitQuadTree( Vec2 pos, Vec2 fwdDirt, float dist )
{
	std::vector<SymmetricQuadTree<GameObject*>*> result;
	std::vector<GameObject*> objResult;
	m_root->GetAllIntersectLeavesNode( result, pos, fwdDirt, dist  );
	for( int i = 0; i < result.size(); i++ ) {
		objResult.insert( objResult.end(), result[i]->objs.begin(), result[i]->objs.end() );
	}
	return objResult;
}

std::string Game::GetRaycastStrategyString() const
{
	switch( m_strategy )
	{
	case NO_INVISIBLE_RAYCAST:
		return " no invisible raycast.";
	case NO_BROAD_CHECK:
		return " no broad check.";
	case IMPLICIT_QUAD_TREE:
		return "implicit quad tree.";
	}
	return "";
}

void Game::SetSelectedObject( GameObject* obj )
{
	m_selectedObj = obj;
}

void Game::Render() const
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	g_theRenderer->DrawCircle( Vec3( m_mousePos), 0.2f, 0.2f, Rgba8::RED );
	RenderObjects();
	RenderUI();
	RenderRaycast();
	if( m_isDebug ) {
		DebugRenderSPTree( m_root );
	}
}

void Game::RenderUI() const
{
	Strings debugInfo;
	debugInfo.push_back( Stringf( "Objects Num: %i", m_objs.size() ) );
	//debugInfo.push_back( Stringf( "mousePos.x: %.2f, mousePos.y: %.2f", m_mousePos.x, m_mousePos.y ));
	debugInfo.push_back( Stringf( "FrameRate: %i", (int)(1 / m_deltaSeconds) ) );
	debugInfo.push_back( Stringf( "Run frame time each frame: %.4f", m_debugTime ));
	debugInfo.push_back( std::string( "raycast strategy is: " + GetRaycastStrategyString() ));
	debugInfo.push_back( Stringf( "Hitnum1: %i", hitNum1 ));
	debugInfo.push_back( Stringf( "Hitnum1: %i", hitNum2 ));
	DebugAddScreenLeftAlignStrings( 0.f, 85.f, Rgba8::WHITE, debugInfo );
}



void Game::LoadAssets()
{
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
}

