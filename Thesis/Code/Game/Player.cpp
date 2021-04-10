#include "Player.hpp"
#include "Game/Enemy.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Camera/CameraSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EventSystem.hpp"

extern InputSystem*		g_theInputSystem;
extern RenderContext*	g_theRenderer;
extern DevConsole*		g_theConsole;
extern CameraSystem*	g_theCameraSystem;
extern EventSystem*		g_theEventSystem;

Player::Player()
{
	m_type = ACTOR_PLAYER;
	m_isPushedByActor = true;
	m_doesPushActor = true;
	static int index = 0;
	m_index = index;
	index++;
	char alpha = 255;
	char intensity = 200;
	if( m_index == 0 ) {
		m_color = Rgba8( intensity, intensity, 0, alpha );
	}
	else if( m_index == 1 ) {
		m_color = Rgba8( 0, intensity, intensity, alpha );
	}
	else if( m_index == 2 ) {
		m_color = Rgba8( intensity, 0, intensity, alpha );
	}
	else if( m_index == 3 ) {
		m_color = Rgba8::WHITE;
	}
}

Player* Player::SpawnPlayerWithPos( Vec2 pos )
{
	Player* tempPlayer = new Player();
	tempPlayer->SetSpeed( 0.f );
	tempPlayer->SetPosition( pos );
	return tempPlayer;
}

void Player::UpdatePlayer( float deltaSeconds, int playerIndex )
{
	if( m_aliveState != AliveState::ALIVE ){ return; }
	CheckInputMethod( playerIndex );
	m_movingDir = Vec2::ZERO;
	
	m_disableInputSeconds -= deltaSeconds;
	if( m_disableInputSeconds < 0.f ) {
		m_isAbleInput = true;
	}

	if( !g_theConsole->IsOpen()) {
		HandleInput( deltaSeconds, playerIndex );
	}

	UpdatePlayerSpeed( deltaSeconds );
	__super::UpdateActor( deltaSeconds, m_color );
}

void Player::UpdatePlayerSpeed( float deltaSeconds )
{
	m_isMoving = false;
	m_isContinousWalk = false;

	if( !IsVec2MostlyEqual( m_movingDir, Vec2::ZERO ) ) {
		m_isMoving = true;
	}

	if( m_isMoving ) {
		m_speed += m_accelerate * deltaSeconds;
		if( m_speed > 1.5f ) {
			m_isContinousWalk = true;
		}
		m_speed = ClampFloat( 0.f, m_maxSpeed, m_speed );
	}
	else {
		m_speed = 0.f;
	}
}

void Player::HandleInput( float deltaSeconds, int playerIndex )
{
	if( !m_isAbleInput ){ return; }
	switch( m_inputState )
	{
	case KEYBOARD_INPUT:
		if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_W ) ) {
			m_movingDir.y = 1.f;
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {
			m_movingDir.y = -1.f;
		}
		
		if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
			m_movingDir.x = 1.f;
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_A ) ) {
			m_movingDir.x = -1.f;
		}
		if( m_movingDir.GetLength() != 0 ) {
			m_movingDir.Normalize();
		}
		if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_LEFT ) ) {
			__super::Shoot( deltaSeconds );
		}
		break;
	case AI_INPUT:
		break; // temp disable ai moving for debug TODO: delete this line
		CheckAIState();
		switch( m_aiState )
		{
		case PLAYER_PATROL:
			FindGoalPatrolPoint( deltaSeconds );
			m_movingDir = m_patrolGoalPos - m_position;
			m_movingDir.Normalize();
			break;
		case PLAYER_ATTACK:
			AIShoot( deltaSeconds );
			break;
		default:
			break;
		}
		break;
	case CONTROLLER_INPUT:
		const XboxController* xboxController = g_theInputSystem->GetXboxController( playerIndex - 1 );
		if( !xboxController->isConnected() ) { return; }


		AnalogJoystick leftJoyStick = xboxController->GetLeftJoystick();
		{
			float magnitude = leftJoyStick.GetMagnitude();
			m_movingDir = Vec2::ZERO;
			if( magnitude > 0 ) {
				m_movingDir = Vec2( magnitude, 0.f );
				m_movingDir.SetAngleDegrees( leftJoyStick.GetAngleDegrees() );
			}
		}
		
		AnalogJoystick rightJoyStick = xboxController->GetRightJoystick();
		{
			float magnitude = rightJoyStick.GetMagnitude();
			if( magnitude > 0 ) {
				m_orientationDegrees = rightJoyStick.GetAngleDegrees();
				m_orientationDegrees = ClampDegressTo360( m_orientationDegrees );
			}
		}

		if( xboxController->GetRightTrigger() > 0 ) {
			__super::Shoot( deltaSeconds );
		}
		break;
	}
}

void Player::RenderPlayer()
{
	if( m_aliveState != ALIVE ){ return; }
	__super::RenderActor();
	Vec2 forwardDirt = Vec2::ONE;
	forwardDirt.SetAngleDegrees( m_orientationDegrees );
	//g_theRenderer->DrawLine( m_position, m_position + forwardDirt * 1.f, 0.1f, m_color );
}

void Player::Die()
{
	SetAliveState( WAIT_FOR_DELETE );
	g_theCameraSystem->PrepareRemoveAndDestroyController( this );
}

bool Player::IsControlledByUser() const
{
	return ( !( m_inputState == AI_INPUT ) );
}

int Player::GetPlayerIndex()
{
	const std::vector<Player*> players = m_map->GetPlayers();
	for( int i = 0; i < players.size(); i++ ) {
		if( players[i] == this ) {
			return i ;
		}
	}
	g_theConsole->DebugErrorf( "player not exist in get player index " );
	return -1;
}

void Player::SetMap( Map* map )
{
	m_map = map;
}

void Player::SetInputControlState( InputControlState state )
{
	m_inputState = state;
}

void Player::DisableInputForSeconds( float seconds )
{
	m_isAbleInput = false;
	g_theEventSystem->SetTimerByFunction( seconds, this, &Player::EnableInput );
	m_disableInputSeconds = seconds;
}

void Player::FakeDeadForSeconds( float seconds )
{
	m_aliveState = WAIT_FOR_REBORN;
	CameraController* controller = g_theCameraSystem->GetCameraControllerWithPlayer( this );
	controller->SetMultipleCameraStableFactorNotStableUntil( 2.f, 0.f );
	g_theCameraSystem->UpdateControllerMultipleFactor( 2.f );

	g_theEventSystem->SetTimerByFunction( seconds, this, &Player::Reborn );
	DisableInputForSeconds( 4.f );
}

bool Player::Reborn( EventArgs& args )
{
	UNUSED( args );
	Vec2 teleportPos = (Vec2)m_map->GetRandomInsideCameraNotSolidTileCoords( g_theCameraSystem->GetNoSplitCamera() );
	SetPosition( teleportPos );
	m_aliveState = ALIVE;
	g_theCameraSystem->UpdateControllerMultipleFactor( 2.f );
	return true;
}

bool Player::EnableInput( EventArgs& args )
{
	UNUSED(args);
	m_isAbleInput = true;
	return true;
}



void Player::CheckInputMethod( int playerIndex )
{
	if( playerIndex == 0 ) {
		m_inputState = KEYBOARD_INPUT;
	}
	else {
		XboxController const* controller = g_theInputSystem->GetXboxController( playerIndex - 1 );
		if( !controller->isConnected() ) {
			m_inputState = AI_INPUT;
		}
		else {
			m_inputState = CONTROLLER_INPUT;
		}
	}
}

void Player::CheckAIState()
{
	std::vector<Enemy*> enemies = m_map->GetEnemies();
	for( int i = 0; i < enemies.size(); i++ ) {
		Enemy* tempEnemy = enemies[i];
		if( tempEnemy == nullptr ){ continue; }

		Vec2 tempEnemyPos = tempEnemy->GetPosition();
		float distSq = GetDistanceSquared2D( tempEnemyPos, m_position );
		if( distSq < (m_activeDistThreshold * m_activeDistThreshold) ) {
			m_target = tempEnemy;
			m_aiState = PLAYER_ATTACK;
			return;
		}
	}
	m_target = nullptr;
	m_aiState = PLAYER_PATROL;
}

void Player::FindGoalPatrolPoint( float deltaSeconds )
{
	m_patrolTotalTime += deltaSeconds;
	float distSq = GetDistanceSquared2D( m_position, m_patrolGoalPos );

	if( distSq < 0.7f || m_patrolTotalTime > 5.f || !m_map->IsPosNotSolid( m_patrolGoalPos ) ) {
		m_patrolTotalTime = 0.f;
		m_patrolGoalPos = (Vec2)m_map->GetRandomInsideNotSolidTileCoords();
	}
}

void Player::AIShoot( float deltaSeconds )
{
	if( m_target == nullptr ) {
		ERROR_AND_DIE( "No enemy in player shoot!" );
	}
	m_movingDir = Vec2::ZERO;
	Vec2 fwdDir = m_target->GetPosition() - m_position;
	fwdDir.Normalize();
	m_orientationDegrees = fwdDir.GetAngleDegrees();

	__super::Shoot( deltaSeconds );
}

