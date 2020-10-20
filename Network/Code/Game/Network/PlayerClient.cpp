#include "PlayerClient.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Network/Server.hpp"
#include "Game/EntityFactory.hpp"
#include "Game/Network/Server.hpp"

extern InputSystem*		g_theInputSystem;
PlayerClient::~PlayerClient()
{

}

PlayerClient::PlayerClient( Server* owner, Camera* camera )
	:Client::Client(owner)
{
	m_gameCamera = camera;
}

void PlayerClient::Startup()
{
	m_player = m_owner->CreateAndPushPlayer();

}

void PlayerClient::Shutdown()
{
	
}

void PlayerClient::Update( float deltaSeconds )
{
}

void PlayerClient::BeginFrame()
{
	g_theInputSystem->BeginFrame();
	PreUpdatePlayer();
}

void PlayerClient::EndFrame()
{

}

void PlayerClient::HandleUserInput()
{

}

void PlayerClient::PreUpdatePlayer()
{
	static Vec3 testPos = Vec3::ZERO;
	static Vec3 forward = Vec3::ZERO;
	static float maxDist = 0.f;
	if( m_player ) {
		testPos = m_player->GetPosition();
		forward = m_gameCamera->GetForwardDirt( m_owner->GetGameConvention() );
		maxDist = 1.f;
	}
	//RaycastTest( testPos, forward, maxDist );
	if( !m_player ) { return; }

	float baseRotSpeed = 30.f;
	float playerOrientation = m_player->m_orientation;

	Vec2 mouseMove = g_theInputSystem->GetRelativeMovementPerFrame();
	float deltaPlayerOrientation = -mouseMove.x * baseRotSpeed;

	playerOrientation += deltaPlayerOrientation;
	m_gameCamera->m_transform.SetYawDegrees( playerOrientation );

	bool isMoving = false;
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_E ) ) {
		isMoving = true;
		m_player->SetMoveDirt( Vec2( 1.f, 0.f ) );
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
		isMoving = true;
		m_player->SetMoveDirt( Vec2( -1.f, 0.f ) );
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_F ) ) {
		isMoving = true;
		m_player->SetMoveDirt( Vec2( 0.f, -1.f ) );
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {
		isMoving = true;
		m_player->SetMoveDirt( Vec2( 0.f, 1.f ) );
	}
	m_player->SetIsMoving( isMoving );
	m_player->SetOrientation( playerOrientation );
// 	m_player->Update( deltaSeconds );
// 
// 	m_gameCamera->SetPosition( m_player->GetPosition() ); // TODO Move to somewhere correct

	//Map* currentMap = m_world->GetCurrentMap();
	//currentMap->RayCast( m_player->GetPosition() + m_gameCamera->GetForwardDirt( m_convension ) * m_player->GetRadius() * 1.5f, m_gameCamera->GetForwardDirt( m_convension ), 1.f );
}
