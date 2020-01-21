#include <windows.h>
#include "Game.hpp"
#include "Game/World.hpp"
#include "Game/App.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/RenderContext.hpp"


const XboxController* playerController = nullptr;
extern App* g_theApp;
extern InputSystem* g_theInputSystem;
extern RenderContext* g_theRenderer;
Texture* temTexture = nullptr;
SpriteSheet* temSpriteSheet = nullptr;


Game::Game( bool isDevelop, Camera* inCamera )
	:m_developMode(isDevelop)
	,m_camera(inCamera)
{
	m_rng=RandomNumberGenerator();
	playerController=&g_theInputSystem->GetXboxController(0);
}

void Game::Startup()
{	
	m_world=new World();
	TestSprite();

}
void Game::Shutdown()
{
	m_world->DestroyEntities();
}

void Game::RunFrame(float deltaTime)
{
	ControlCollision();
	Update(deltaTime);
}

void Game::CheckGameStates()
{
	CheckIfPause();
	CheckIfDeveloped();
	CheckIfDebugNoClip();
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F4 ) ) {
		m_debugCamera=!m_debugCamera;
		if(m_debugCamera){
			m_camera->SetOrthoView(Vec2(0,0),Vec2((float)(30*1.67),30.f));
		}
		else{
			m_camera->SetOrthoView(Vec2( 0, 0 ), Vec2( 16, 9 ) );
		}
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F8 ) ) {
		g_theApp->ResetGame();
	}
}

void Game::CheckIfPause()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P)){
		m_isPause=!m_isPause;
	}
	
}

void Game::CheckIfDeveloped()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F1 ) ) {
		m_developMode=!m_developMode;
	}
}

void Game::CheckIfDebugNoClip()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F3 ) ) {
		m_noClip=!m_noClip;
	}
}

void Game::Render() const
{
	if( m_isAttractMode ) {

	}
	else{
		m_world->Render();
	}
}

void Game::ControlCollision()
{

}

void Game::RenderUI() const
{
}

void Game::Reset()
{	
	Startup();
}

void Game::Update(float deltaTime)
{
	CheckGameStates();
	if(m_isPause){return;}
	if(m_isAttractMode){	
	}
	else{
		UpdateWorld(deltaTime);
	}
}

void Game::TestSprite()
{
	const char* temPath = "Data/Images/SpriteSheetStarterKit/Test_SpriteSheet8x2.png";
	temTexture = g_theRenderer->CreateOrGetTextureFromFile(temPath);
	temSpriteSheet = new SpriteSheet(*temTexture, IntVec2(8,2));

}

void Game::UpdateCamera( float deltaTime )
{
	UNUSED (deltaTime);
}

void Game::UpdateWorld( float deltaTime )
{
	m_world->Update(deltaTime);
}

