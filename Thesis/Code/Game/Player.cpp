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

Player::Player( int index, bool isBoss )
{
	if( !isBoss ) {
		m_type = ACTOR_PLAYER;
		m_isPushedByActor = true;
		m_doesPushActor = true;
		m_index = index;
		unsigned char alpha = 255;
		unsigned char intensity = 200;
		if( m_index == 0 ) {
			m_color = Rgba8( 255,255,255, alpha );
		}
		else if( m_index == 1 ) {
			m_color = Rgba8( 0, intensity, intensity, alpha );
		}
		else if( m_index == 2 ) {
			m_color = Rgba8( intensity, 0, intensity, alpha );
		}
		else if( m_index == 3 ) {
			m_color = Rgba8( 255,125,125, alpha );
		}
		m_togetherColor = Rgba8( 50, 50, 50, alpha );
		m_hp = 1000.f;
	}
	else {
		m_type = ACTOR_BOSS;
		m_isPushedByActor = true;
		m_doesPushActor = true;
		m_index = index;
		m_color = Rgba8::WHITE;
		{

		}
		m_hp = 200.f;
		m_baseOBBSize = Vec2( 6.f );
		m_barrelOBBSize = Vec2( 4.f );
	}
}

Player* Player::SpawnPlayerWithPos( Vec2 pos, int index )
{
	Player* tempPlayer = new Player( index );
	tempPlayer->SetSpeed( 0.f );
	tempPlayer->SetPosition( pos );
	return tempPlayer;
}

Player* Player::SpawnBossWithPos( Vec2 pos, int index )
{
	Player* tempBoss = new Player( index, true );
	tempBoss->SetSpeed( 0.f );
	tempBoss->SetPosition( pos );
	tempBoss->m_isBoss = true;
	return tempBoss;
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

void Player::UpdateBoss( float deltaSeconds )
{
	// Find Target
	// Attack
	static float attack1TotalSeconds = 0.f;
	static float attack2TotalSeconds = 0.f;
	static float attack3TotalSeconds = 0.f;
	float attack1Cooldown		= 3.f;
	float attack2Cooldown		= 4.f;
	attack1TotalSeconds += deltaSeconds;
	attack2TotalSeconds += deltaSeconds;
	attack3TotalSeconds += deltaSeconds;
	if( attack1TotalSeconds > attack1Cooldown ) {
		BossAttack1();
		attack1TotalSeconds = 0.f;
	}
	else if( attack2TotalSeconds > attack2Cooldown ) {
		BossAttack2();
		attack2TotalSeconds = 0.f;
	}
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
			//Shoot( m_damage );
		}
		break;
	case AI_INPUT:
		CheckAIState();
		switch( m_aiState )
		{
		case PLAYER_PATROL:
			FindGoalPatrolPoint( deltaSeconds );
			m_movingDir = m_patrolGoalPos - m_position;
			m_movingDir.Normalize();
			break;
		case PLAYER_ATTACK:
			AIShoot( m_damage );
			break;
		default:
			break;
		}
		break;
	case CONTROLLER_INPUT:
		const XboxController* xboxController;
		if( m_map->GetName() == "level2" ) {
			xboxController = g_theInputSystem->GetXboxController( playerIndex );
		}
		else {
			xboxController = g_theInputSystem->GetXboxController( playerIndex - 1 );
		}
		if( !xboxController ){ return; }
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
			Shoot( m_damage );
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
	if( m_map->GetName() == "level2" ) {
		RenderAimingLazer();
	}
}

void Player::RenderAimingLazer()
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	float thick = 0.15f;
	float halfThick = thick / 2;
	Vec2 direction = Vec2::ONE_ZERO;
	direction.SetAngleDegrees( m_orientationDegrees );
	Vec2 perpendicularDirt = Vec2( -direction.y, direction.x );
	Vec2 leftTop = m_position + ( perpendicularDirt * halfThick );
	Vec2 leftdown = m_position - ( perpendicularDirt * halfThick );
	Vec2 rightTop = m_position + direction * 40.f + ( perpendicularDirt * halfThick );
	Vec2 rightdown = m_position + direction * 40.f - ( perpendicularDirt * halfThick );
	Vec2 tem_uv = Vec2( 0.f, 0.f );
	Vertex_PCU line[6]={
		Vertex_PCU( Vec3( rightTop.x,rightTop.y,0 ),	Rgba8( 255, 0, 0, 0 ),	tem_uv ),
		Vertex_PCU( Vec3( rightdown.x,rightdown.y,0 ),	Rgba8( 255, 0, 0, 0 ),	tem_uv ),
		Vertex_PCU( Vec3( leftTop.x,leftTop.y,0 ),		Rgba8( 255, 0, 0, 100 ),	tem_uv ),
		Vertex_PCU( Vec3( leftTop.x,leftTop.y, 0 ),		Rgba8( 255, 0, 0, 100 ),	tem_uv ),
		Vertex_PCU( Vec3( leftdown.x,leftdown.y, 0 ),	Rgba8( 255, 0, 0, 100 ),	tem_uv ),
		Vertex_PCU( Vec3( rightdown.x,rightdown.y, 0 ), Rgba8( 255, 0, 0, 0 ),	tem_uv )
	};
	g_theRenderer->DrawVertexArray( 6, line );
}

void Player::RenderBoss()
{
	__super::RenderActor();
}

void Player::BossAttack1()
{
	for( int i = 0; i < 18; i++ ) {
		float degrees = i * 10.f + 90.f;
		Vec2 dirt = Vec2::ONE_ZERO;
		dirt.SetAngleDegrees( degrees );
		m_map->SpawnNewProjectile( m_type, m_position, dirt , m_color );
	}
}

void Player::BossAttack2()
{

}

void Player::BossAttack3()
{

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

void Player::SetDamage( float damage )
{
	m_damage = damage;
}

void Player::SetMaxSpeed( float maxSpeed )
{
	m_maxSpeed = maxSpeed;
}

void Player::SetIsStronger( bool isStronger )
{
	m_isStronger = isStronger;
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



void Player::Shoot( float damage )
{
	if( m_shootTimer < m_shootCoolDown ) { return; }

	m_shootTimer = 0.f;
	Vec2 projectileDirt = Vec2::ONE;
	projectileDirt.SetAngleDegrees( m_orientationDegrees );
	if( m_isStronger ) {
		m_map->SpawnNewProjectileWithDamage( m_type, m_position, projectileDirt, m_togetherColor, damage );
	}
	else {
		m_map->SpawnNewProjectile( m_type, m_position, projectileDirt, m_color );
	}
}

void Player::CheckInputMethod( int playerIndex )
{
	if( m_map->GetName() == "level2" ) {
		m_inputState = CONTROLLER_INPUT;
	}
	else {
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

void Player::AIShoot( float damage )
{
	if( m_target == nullptr ) {
		ERROR_AND_DIE( "No enemy in player shoot!" );
	}
	m_movingDir = Vec2::ZERO;
	Vec2 fwdDir = m_target->GetPosition() - m_position;
	fwdDir.Normalize();
	m_orientationDegrees = fwdDir.GetAngleDegrees();

	__super::Shoot( damage );
}

