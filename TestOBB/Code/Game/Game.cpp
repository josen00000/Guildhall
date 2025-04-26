#include <windows.h>
#include <string>
#include "Game.hpp"
#include "Game/App.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/EngineCommon.hpp"
//#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/renderer/ObjectReader.hpp"

extern App* g_theApp;
extern InputSystem* g_theInputSystem;
extern RenderContext* g_theRenderer;
extern Camera* g_UICamera;
extern Camera* g_gameCamera;
extern BitmapFont* g_squirrelFont;
Texture* temTexture = nullptr;
EventSystem* g_eventSystem = new EventSystem();


Game::Game( Camera* gameCamera, Camera* UICamera )
	:m_gameCamera( gameCamera )
	,m_UICamera(UICamera)
{
	m_rng=RandomNumberGenerator();
}

void Game::Startup()
{	
	LoadGameAsset();
	CreateRandomOBB();
	CreateOtherShapes();
	CreateRandomCapsule();

	// vulkan mesh test
	ObjectReader* testObjReader = new ObjectReader("Data/Model/viking_room.obj");
	m_mesh = new GPUMesh( g_theRenderer, VERTEX_TYPE_PCU );
	testObjReader->GenerateGPUMesh( *m_mesh );
	m_meshTransform.SetPosition( Vec3( 0.f, 0.f, -1.f ) );
	//m_meshTexture = g_theRenderer->CreateTextureFromFile( "Data/Model/viking_room.png" );
}

void Game::Shutdown()
{
}

void Game::RunFrame(float deltaTime)
{
	Update(deltaTime);
}

void Game::Render() const
{

	g_theRenderer->SetModelMatrix( m_meshTransform.ToMatrix() );
	g_theRenderer->DrawMesh( m_mesh );
	RenderOBBs();
	//RenderCapsules();
	//RenderOtherShapes();
	//RenderMouse( *g_gameCamera );
	//RenderNearestPoints();
}


void Game::RenderUI() const
{
	g_theRenderer->DrawLine( Vec2( 40, 40 ), Vec2( 80, 80 ), 2, Rgba8::WHITE );
}


void Game::EndFrame()
{
	// TODO: Add what need to do when the frame end
}

void Game::Update(float deltaTime)
{
	CheckIfExit();

	TestMouse();
	std::string testMousestring = "m_mousePos is " + std::to_string(m_mousePos.x) + " " + std::to_string(m_mousePos.y);
	TestOBBOverlap();
	UpdateOBBs( deltaTime );
	UpdateOtherShapes();
	UpdateCapsule();
	
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_C ) ) {
		RecreateShapes();
	}
}


void Game::TestMouse()
{
	HWND handle = (HWND)g_theWindow->GetHandle();
	Vec2 mousePosInClient = g_theInputSystem->GetNormalizedMousePosInClient( handle );
	m_mousePos = (Vec2)m_gameCamera->ClientToWorld( mousePosInClient, 1.f );
}

void Game::TestOBBOverlap()
{
	for(int obbIndex = 0; obbIndex < m_obbNum; obbIndex++ ){
		OBB2 tempOBB = m_OBBs[obbIndex];
		bool isOverlap = DoOBBAndOBBOVerlap2D( tempOBB, m_mouseOBB );
		if(isOverlap){
			m_mouseOBBOverlaped = true;
			return;
		}
	}
	m_mouseOBBOverlaped = false;
}

void Game::RenderMouse( const Camera& camera) const
{
	g_theRenderer->SetDiffuseTexture(nullptr);
	AABB2 orthoBounds( camera.GetBottomLeftWorldPos2D(), camera.GetTopRightWorldPos2D() );
	Vec3 mouseDrawPos3 = Vec3( m_mousePos );
	g_theRenderer->DrawCircle(mouseDrawPos3, 1, 1, Rgba8::WHITE);
}

void Game::LoadGameAsset()
{
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_ESC ) ) {
		g_theApp->HandleQuitRequested();
	}
}

void Game::CreateRandomOBB()
{
	m_OBBs.clear();
	for( int obbIndex = 0; obbIndex < m_obbNum; obbIndex++ ){
		Vec2 tempDimension = m_rng.RollRandomVec2InRange( Vec2(5, 5), Vec2(10, 10) );
		
		Vec2 tempCenter = m_rng.RollRandomVec2InRange( g_gameCamera->GetBottomLeftWorldPos2D(), g_gameCamera->GetTopRightWorldPos2D() );
		tempCenter = Vec2(30,30); //testing
		float tempDegree = m_rng.RollRandomFloatInRange( 0, 360 );
		OBB2 tempOBB = OBB2( tempDimension, tempCenter, tempDegree );
		m_OBBs.push_back(tempOBB);
	}
	//m_OBBs.push_back(m_OBBs[0]);
	m_mouseOBB = OBB2( Vec2(10, 10), Vec2(31,11), Vec2(1,0) );
}

void Game::CreateRandomCapsule()
{
	m_capsules.clear();
	for( int capsuleIndex = 0; capsuleIndex < m_capNum; capsuleIndex++ ) {
		Vec2 tempStart = m_rng.RollRandomVec2InRange( g_gameCamera->GetBottomLeftWorldPos2D(), g_gameCamera->GetTopRightWorldPos2D() );
		float length = m_rng.RollRandomFloatLessThan( 10 );
		Vec2 direction = m_rng.RollRandomVec2InRange( Vec2( -1, -1 ), Vec2::ONE );
		float radius = m_rng.RollRandomFloatLessThan( 10 );
		Capsule2 tempCapsule = Capsule2( tempStart, direction, length, radius );
		m_capsules.push_back( tempCapsule );
	}
}

void Game::CreateOtherShapes()
{
	//Disc
	m_center = m_rng.RollRandomVec2InRange( g_gameCamera->GetBottomLeftWorldPos2D(), g_gameCamera->GetTopRightWorldPos2D() );
	m_radius = m_rng.RollRandomFloatInRange( 3, 5 );
	
	//line
	Vec2 tempStart = m_rng.RollRandomVec2InRange( g_gameCamera->GetBottomLeftWorldPos2D(), g_gameCamera->GetTopRightWorldPos2D() );
	Vec2 tempEnd = m_rng.RollRandomVec2InRange( g_gameCamera->GetBottomLeftWorldPos2D(), g_gameCamera->GetTopRightWorldPos2D() );
	m_line = LineSegment2(tempStart, tempEnd);

	// aabb
	Vec2 min = m_rng.RollRandomVec2InRange( g_gameCamera->GetBottomLeftWorldPos2D(), g_gameCamera->GetTopRightWorldPos2D() );
	float maxX = m_rng.RollRandomFloatInRange( min.x +5, min.x + 10);
	float maxY = m_rng.RollRandomFloatInRange( min.y +5, min.y + 10);
	m_box = AABB2(min.x, min.y, maxX, maxY );
}


void Game::RecreateShapes()
{
	m_OBBs.clear();
	m_capsules.clear();
	CreateRandomOBB();
	CreateRandomCapsule();
	CreateOtherShapes();
}

void Game::UpdateOBBs( float deltaSeconds)
{
	UNUSED(deltaSeconds);
	m_OBBVertices.clear();
	m_nearestPoints.clear();
	AABB2 orthoBounds( g_gameCamera->GetBottomLeftWorldPos2D(), g_gameCamera->GetBottomLeftWorldPos2D() );
	Vec2 mouseDrawPos = orthoBounds.GetPointAtUV( m_mousePos );
	for( int obbIndex = 0; obbIndex < m_obbNum; obbIndex++ ) {
		OBB2 tempOBB = m_OBBs[obbIndex];
		if(IsPointInsideOBB2D(mouseDrawPos, tempOBB)){
			OBB2::AddVertsForOBB2D( m_OBBVertices, tempOBB, Rgba8::WHITE, Vec2::ZERO, Vec2::ZERO );
		}
		else{
			OBB2::AddVertsForOBB2D( m_OBBVertices, tempOBB, Rgba8::BLUE, Vec2::ZERO, Vec2::ZERO );
		}
		
		Vec2 tempPoint = tempOBB.GetNearestPoint( mouseDrawPos );
		std::string pointPosString = "nearest point position is "+ std::to_string(tempPoint.x) + "  " + std::to_string(tempPoint.y);
		//m_testConsole->PrintString( Rgba8::RED, pointPosString );
		m_nearestPoints.push_back(tempPoint);
	}
	//Vec2 mouseOBBPos = m_mouseOBB.m_center;
	//mouseOBBPos.x += 10 * deltaSeconds;
	//m_mouseOBB.SetCenter( mouseOBBPos );
	static float orientation = m_mouseOBB.GetOrientationDegrees();
	if( g_theInputSystem->IsKeyDown (KEYBOARD_BUTTON_ID_R)){
		orientation+= 1;
	}
	m_mouseOBB.SetCenter( mouseDrawPos );
	m_mouseOBB.SetOrientationDegrees( orientation );
	Rgba8 obbColor = Rgba8(255,0,0,100);
	if( m_mouseOBBOverlaped){
		obbColor.a = 255;
	}
	AppendVertsForOBB2D( m_OBBVertices, m_mouseOBB, obbColor, Vec2::ZERO, Vec2::ZERO );
}

void Game::UpdateCapsule()
{
	m_capsuleVertices.clear();
	for( int capsuleIndex = 0; capsuleIndex < m_capNum; capsuleIndex++ ) {
		Capsule2 tempCapsule = m_capsules[capsuleIndex];
		AABB2 orthoBounds( g_gameCamera->GetBottomLeftWorldPos2D(), g_gameCamera->GetTopRightWorldPos2D() );
 		Vec2 mouseDrawPos = orthoBounds.GetPointAtUV( m_mousePos );
		if(IsPointInsideCapsule2D(mouseDrawPos, tempCapsule.m_start, tempCapsule.m_end, tempCapsule.m_radius)){
			AppendVertsForCapsule2D( m_capsuleVertices, tempCapsule, Rgba8::BLUE);
		}
		else{
			AppendVertsForCapsule2D( m_capsuleVertices, tempCapsule, Rgba8::WHITE);
		}

 		Vec2 tempPoint = tempCapsule.GetNearestPoint( mouseDrawPos );
 		m_nearestPoints.push_back( tempPoint );
	}
}

void Game::UpdateOtherShapes()
{
	m_discVertices.clear();
	m_boxVertices.clear();
	AABB2 orthoBounds( g_gameCamera->GetBottomLeftWorldPos2D(), g_gameCamera->GetTopRightWorldPos2D() );
	Vec2 mouseDrawPos = orthoBounds.GetPointAtUV( m_mousePos );
	Rgba8 discColor = Rgba8::BLUE;
	Rgba8 aabbColor = Rgba8::BLUE;
	if( IsPointInsideDisc( mouseDrawPos, m_center, m_radius)){
		discColor = Rgba8::WHITE;
	}
	else{
		discColor = Rgba8::BLUE;
	}
	if( IsPointInsideAABB2D( mouseDrawPos,m_box ) ) {
		aabbColor = Rgba8::WHITE;
	}
	else {
		aabbColor = Rgba8::BLUE;
	}
	AppendVertsForCircle2D( m_discVertices, m_center, m_radius, discColor );
	//aabb
	AppendVertsForAABB2D( m_boxVertices, m_box, aabbColor, Vec2::ZERO, Vec2::ZERO );
	Vec2 discPoint = GetNearestPointOnDisc2D( mouseDrawPos, m_center, m_radius );
	Vec2 aabbPoint = GetNearestPointOnAABB2D( mouseDrawPos, m_box );
	Vec2 linePoint = GetNearestPointOnLineSegment2D( mouseDrawPos, m_line.m_start, m_line.m_end );
	m_nearestPoints.push_back( discPoint );
	m_nearestPoints.push_back( aabbPoint );
	m_nearestPoints.push_back( linePoint );

}

void Game::RenderOBBs() const
{
	g_theRenderer->SetDiffuseTexture(nullptr);
	g_theRenderer->DrawVertexVector(m_OBBVertices);
}

void Game::RenderCapsules() const
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	g_theRenderer->DrawVertexVector( m_capsuleVertices );
}

void Game::RenderOtherShapes() const
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	g_theRenderer->DrawLine(m_line.m_start, m_line.m_end, 0.1f, Rgba8::WHITE);
	g_theRenderer->DrawVertexVector( m_discVertices );
	g_theRenderer->DrawVertexVector( m_boxVertices );
}

void Game::RenderNearestPoints() const
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	for( int pointIndex = 0; pointIndex < m_nearestPoints.size(); pointIndex++ ) {
		g_theRenderer->DrawCircle( Vec3( m_nearestPoints[pointIndex] ), 1, 1, Rgba8::RED );
	}
}





