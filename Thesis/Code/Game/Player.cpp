#include "Player.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern InputSystem*		g_theInputSystem;
extern RenderContext*	g_theRenderer;

Player::Player()
{
	m_type = ACTOR_PLAYER;
	m_color = Rgba8::RED;
}

Player* Player::SpawnPlayerWithPos( Vec2 pos )
{
	Player* tempPlayer = new Player();
	tempPlayer->SetPosition( pos );
	return tempPlayer;
}

void Player::UpdatePlayer( float deltaSeconds )
{
	HandleInput( deltaSeconds );
	__super::UpdateActor( deltaSeconds );
}

void Player::HandleInput( float deltaSeconds )
{
	m_movingDirt = Vec2::ZERO;
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_W ) ) {
		m_movingDirt.y = 1.f;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {
		m_movingDirt.y = -1.f;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
		m_movingDirt.x = 1.f;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_A ) ) {
		m_movingDirt.x = -1.f;
	}
	if( m_movingDirt.GetLength() != 0 ) {
		m_movingDirt.Normalize();
	}
	if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_LEFT ) ) {
		Shoot();
	}
}

void Player::RenderPlayer()
{
	__super::RenderActor();
	Vec2 forwardDirt = Vec2::ONE;
	forwardDirt.SetAngleDegrees( m_orientationDegrees );
	g_theRenderer->DrawLine( m_position, m_position + forwardDirt * 1.f, 0.1f, m_color );
}

void Player::SetMap( Map* map )
{
	m_map = map;
}

