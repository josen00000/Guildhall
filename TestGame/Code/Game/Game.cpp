#include <windows.h>
#include <string>
#include "Game.hpp"
#include "Game/App.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
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
	g_theRenderer->DrawVertexVector(m_alignTextVertices);
	RenderMouse( *g_camera );
}


void Game::RenderUI() const
{
	m_testConsole->Render(*g_theRenderer, *g_UICamera, 1);
}

void Game::Update(float deltaTime)
{
	TestMouse();
	std::string testMousestring = "m_mousePos is " + std::to_string(m_mousePos.x) + " " + std::to_string(m_mousePos.y);
	m_testConsole->PrintString(Rgba8::WHITE, testMousestring);
	UpdateAlignText(deltaTime);
	static bool ableDevConsole = false;
	static float currentTime = 0;
	currentTime += deltaTime;
	if(currentTime > 3){
		ableDevConsole = true;
	}
	if(ableDevConsole){
		m_testConsole->SetIsOpen(true);
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
	Vec2 mouseNormalizedPos = g_theInputSystem->GetNormalizedMousePos();
	AABB2 orthoBounds(camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight());
	Vec2 mouseDrawPos = orthoBounds.GetPointAtUV(mouseNormalizedPos); 
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

void Game::RenderMouse( const Camera& camera) const
{
	AABB2 orthoBounds( camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight() );
	Vec2 mouseDrawPos = orthoBounds.GetPointAtUV( m_mousePos );
	g_theRenderer->DrawCircle(m_mousePos, 0.1f, 0.1f, Rgba8::WHITE);
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
	m_testFont->AddVertsForTextInBox2D(m_alignTextVertices, m_alignBox, 1.5, temString, Rgba8::BLUE, 1, m_alignPos);
}

void Game::UpdateTestString()
{
	for( int i = 0; i < 10; i++ ) {
		m_testConsole->PrintString(Rgba8::WHITE, "Update Test String");
	}
}

void Game::LoadGameAsset(){
	//g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/SpriteSheetStarterKit/Test_SpriteSheet8x2.png");
	m_testFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
}



