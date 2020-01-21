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

extern App* g_theApp;
extern InputSystem* g_theInputSystem;
extern RenderContext* g_theRenderer;
extern Camera* g_UICamera;
extern Camera* g_camera;
Texture* temTexture = nullptr;
SpriteSheet* temSpriteSheet = nullptr;
EventSystem* g_eventSystem = new EventSystem();

Game::Game(Camera* inCamera)
	:m_camera(inCamera)
{
	m_rng=RandomNumberGenerator();
}

void Game::Startup()
{	
	LoadGameAsset();
	InitialAlignText();
	TestSprite();
	TestImage();
	CreateRandomOBB();
	CreateOtherShapes();
	CreateRandomCapsule();
	TestSplitString();
	TestSetFromText();
	m_testConsole = new DevConsole(m_testFont);


	

}

void Game::Shutdown()
{
	delete m_testConsole;
}

void Game::RunFrame(float deltaTime)
{
	Update(deltaTime);
}

void Game::Render() const
{
	//g_theRenderer->DrawLine( Vec2( 40, 40 ), Vec2( 80, 80 ), 2, Rgba8::WHITE );
	//g_theRenderer->DrawCircle(Vec3(80,80,0), 20, 20,Rgba8::WHITE);
	
	RenderOBBs();
	RenderCapsules();
	RenderOtherShapes();
	RenderMouse( *g_camera );
	RenderNearestPoints();

	g_theRenderer->BindTexture(m_testFont->GetTexture());
	g_theRenderer->DrawVertexVector(m_alignTextVertices);

}


void Game::RenderUI() const
{
	g_theRenderer->DrawLine( Vec2( 40, 40 ), Vec2( 80, 80 ), 2, Rgba8::WHITE );

	m_testConsole->Render(*g_theRenderer, *g_UICamera, 1);
}

void Game::PrintSomething( const std::string stringToPrint )
{
	m_testConsole->PrintString( Rgba8::RED, stringToPrint);
}

void Game::Update(float deltaTime)
{
	TestMouse();
	std::string testMousestring = "m_mousePos is " + std::to_string(m_mousePos.x) + " " + std::to_string(m_mousePos.y);
	//m_testConsole->PrintString(Rgba8::WHITE, testMousestring);
	UpdateAlignText(deltaTime);
	TestOBBOverlap();
	UpdateOBBs( deltaTime );
	UpdateOtherShapes();
	UpdateCapsule();
	
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_C ) ) {
		RecreateShapes();
	}

	// DevConsole
	/*
	static float currentTime = 0;
	currentTime += deltaTime;
	if(currentTime > 3){
		/ableDevConsole = true;
	}
	if(ableDevConsole){
		m_testConsole->SetIsOpen(true);
	}*/
	static bool ableDevConsole = false;
	if( g_theInputSystem->GetKeyBoardController().GetButtonState( KEYBOARD_BUTTON_ID_P ).WasJustPressed() ) {
		ableDevConsole = !ableDevConsole;
		m_testConsole->SetIsOpen(ableDevConsole);
	}
}

void Game::TestSprite()
{
	temSpriteSheet = new SpriteSheet(*temTexture, IntVec2(8,2));

}

void Game::TestImage()
{
	//Image *temImage = new Image("Data/Images/Test_StbiFlippedAndOpenGL.png");
	//Image *temImage1 = new Image("Data/Images/Test_StbiAndDirectX.png");

}

void Game::TestDrawMouse( const Camera& camera)
{
	UNUSED( camera );
	/*Vec2 mouseNormalizedPos = g_theInputSystem->UpdateMouse();
	AABB2 orthoBounds(camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight());
	Vec2 mouseDrawPos = orthoBounds.GetPointAtUV(mouseNormalizedPos); */
}

void Game::TestSetFromText()
{
	Vec2 testVec2 = Vec2();
	testVec2.SetFromText("6,4");
	testVec2.SetFromText(" -.6 , 0.44 ");
}

void Game::TestMouse()
{
	m_mousePos = g_theInputSystem->GetNormalizedMousePos();

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
	g_theRenderer->BindTexture(nullptr);
	AABB2 orthoBounds( camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight() );
	Vec2 m_mouseDrawPos = orthoBounds.GetPointAtUV( m_mousePos );
	Vec3 mouseDrawPos3 = Vec3( m_mouseDrawPos );
	g_theRenderer->DrawCircle(mouseDrawPos3, 1, 1, Rgba8::WHITE);
}

void Game::TestSplitString()
{
	Strings s1 = SplitStringOnDelimiter( "Amy,Bret,Carl", "," ); // split into 3 substrings: "Amy", "Bret", "Carl"
	Strings s2 = SplitStringOnDelimiter( " -7.5, 3 ", "," );     // split into 2: " -7.5" and " 3 " (including whitespace!)
	Strings s3 = SplitStringOnDelimiter( "3~7", "~" );           // split into 2: "3" and "7"
	Strings s4 = SplitStringOnDelimiter( "255, 128, 40", "," );  // split into 3: "255", " 128", and " 40" (including spaces!)
	Strings s5 = SplitStringOnDelimiter( "apple", "/" );         // split into 1: "apple"
	Strings s6 = SplitStringOnDelimiter( "8/2/1973", "/" );      // split into 3: "8", "2", and "1973"
	Strings s7 = SplitStringOnDelimiter( ",,", "," );            // split into 3: "", "", and ""
	Strings s8 = SplitStringOnDelimiter( ",,Hello,,", "," );     // split into 5: "", "", "Hello", "", and ""
	Strings s9 = SplitStringOnDelimiter( "", "," );              // split into 1: ""

	
}

void Game::InitialAlignText()
{
	//m_alignTestString = "alignment Testing";
}

void Game::UpdateAlignText( float deltaTime )
{	m_alignTextVertices.clear();
	
	static int coe = 1;
	if(m_alignPos.x > 1 || m_alignPos.x < 0 ){
		coe *= -1;
	}
	m_alignPos.x += coe * deltaTime * m_alignMoveSpeed;
	m_alignPos.y -= coe * deltaTime * m_alignMoveSpeed;
	


	std::string temString = "testing";
	temString = temString + "  alignPos (" + std::to_string( m_alignPos.x) + " , " + std::to_string( m_alignPos.y) + " )";
	temString = "Try press button R and C, R to rotate the mouse obb. C to recreate the shapes";
	m_testFont->AddVertsForTextInBox2D(m_alignTextVertices, m_alignBox, 1.5, temString, Rgba8::WHITE, 1, Vec2::ZERO);
}

void Game::LoadGameAsset(){
	//g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/SpriteSheetStarterKit/Test_SpriteSheet8x2.png");
	m_testFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
}

void Game::CreateRandomOBB()
{
	m_OBBs.clear();
	for( int obbIndex = 0; obbIndex < m_obbNum; obbIndex++ ){
		Vec2 tempDimension = m_rng.RollRandomVec2InRange( Vec2(5, 5), Vec2(10, 10) );
		
		Vec2 tempCenter = m_rng.RollRandomVec2InRange( g_camera->GetOrthoBottomLeft(), g_camera->GetOrthoTopRight() );
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
		Vec2 tempStart = m_rng.RollRandomVec2InRange( g_camera->GetOrthoBottomLeft(), g_camera->GetOrthoTopRight() );
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
	m_center = m_rng.RollRandomVec2InRange( g_camera->GetOrthoBottomLeft(), g_camera->GetOrthoTopRight() );
	m_radius = m_rng.RollRandomFloatInRange( 3, 5 );
	
	//line
	Vec2 tempStart = m_rng.RollRandomVec2InRange( g_camera->GetOrthoBottomLeft(), g_camera->GetOrthoTopRight() );
	Vec2 tempEnd = m_rng.RollRandomVec2InRange( g_camera->GetOrthoBottomLeft(), g_camera->GetOrthoTopRight() );
	m_line = LineSegment2(tempStart, tempEnd);

	// aabb
	Vec2 min = m_rng.RollRandomVec2InRange( g_camera->GetOrthoBottomLeft(), g_camera->GetOrthoTopRight() );
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
	AABB2 orthoBounds( g_camera->GetOrthoBottomLeft(), g_camera->GetOrthoTopRight() );
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
		AABB2 orthoBounds( g_camera->GetOrthoBottomLeft(), g_camera->GetOrthoTopRight() );
 		Vec2 mouseDrawPos = orthoBounds.GetPointAtUV( m_mousePos );
		if(IsPointInsideCapsule2D(mouseDrawPos, tempCapsule.m_start, tempCapsule.m_end, tempCapsule.m_radius)){
			AppendVertsForCapsule2D( m_capsuleVertices, tempCapsule, Rgba8::BLUE, Vec2::ZERO, Vec2::ZERO );
		}
		else{
			AppendVertsForCapsule2D( m_capsuleVertices, tempCapsule, Rgba8::WHITE, Vec2::ZERO, Vec2::ZERO );
		}

 		Vec2 tempPoint = tempCapsule.GetNearestPoint( mouseDrawPos );
 		//std::string pointPosString = "nearest point position is "+ std::to_string( tempPoint.x ) + "  " + std::to_string( tempPoint.y );
 		//m_testConsole->PrintString( Rgba8::RED, pointPosString );
 		m_nearestPoints.push_back( tempPoint );
	}
}

void Game::UpdateOtherShapes()
{
	m_discVertices.clear();
	m_boxVertices.clear();
	AABB2 orthoBounds( g_camera->GetOrthoBottomLeft(), g_camera->GetOrthoTopRight() );
	Vec2 mouseDrawPos = orthoBounds.GetPointAtUV( m_mousePos );
	Rgba8 discColor = Rgba8::BLUE;
	Rgba8 aabbColor = Rgba8::BLUE;
	if(IsPointInDisc( mouseDrawPos, m_center, m_radius)){
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
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexVector(m_OBBVertices);
}

void Game::RenderCapsules() const
{
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawVertexVector( m_capsuleVertices );
}

void Game::RenderOtherShapes() const
{
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawLine(m_line.m_start, m_line.m_end, 0.1f, Rgba8::WHITE);
	g_theRenderer->DrawVertexVector( m_discVertices );
	g_theRenderer->DrawVertexVector( m_boxVertices );
}

void Game::RenderNearestPoints() const
{
	g_theRenderer->BindTexture( nullptr );
	for( int pointIndex = 0; pointIndex < m_nearestPoints.size(); pointIndex++ ) {
		g_theRenderer->DrawCircle( Vec3( m_nearestPoints[pointIndex] ), 1, 1, Rgba8::RED );
	}
}





