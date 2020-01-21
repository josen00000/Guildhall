#include "Player.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/World.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"


extern RenderContext* g_theRenderer;
extern InputSystem* g_theInputSystem;
extern AudioSystem* g_theAudioSystem;
extern Game* g_theGame;


Player::Player(Map* map, Vec2 spawnPosition)
	:Entity(map,spawnPosition,ENTITY_TYPE_PLAYER, ENTITY_FACTION_GOOD)
{
	m_angularVelocity = 90.f;
	m_physicsRadius = 0.2f;
	m_cosmeticRadius=0.5f;
	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullet = true;
	m_health = 3; // should be 3
	CreateTank();
}

Player::~Player()
{
	delete m_tankTexture;
	delete m_barrelTexture;
	m_tankTexture=nullptr;
	m_barrelTexture=nullptr;
}

void Player::Update( float deltaSeconds )
{
	UpdateTank(deltaSeconds);
	UpdateBarrel(deltaSeconds);
	UpdateXboxVibration(deltaSeconds);
	Entity::Update(deltaSeconds);
}

 void Player::Render()const
{
	RenderTank();
	RenderBarrel();
	if(g_theGame->m_developMode){
		Entity::Render();
	}
}

void Player::Die()
{
	g_theAudioSystem->PlaySound(playerDie);
	if(m_map->m_world->m_playerLife == 0 ){
		m_map->m_world->m_playerLife--;
	}
	m_map->SpawnNewExplosion(m_position, PLAYER_EXPLOSION_RADIU, PLAYER_EXPLOSION_DURATION);
	m_map->InitializeDeadScene();
	g_theGame->m_isPlayerDead = true;
	g_theGame->m_vibrationIntense = 1;
	m_map->m_playerDeadPos = m_position;
	Entity::Die();
}

void Player::TakeDamage()
{
	g_theAudioSystem->PlaySound(playerHit);
	Entity::TakeDamage();
	if( m_health <= 0 ) {
		Die();
	}
}

void Player::UpdateTank( float deltaSeconds )
{
	//Check the input
	const XboxController xboxController = g_theInputSystem->GetXboxController(0);
	//Check fire button click
	KeyButtonState xboxButtonStateA = xboxController.GetButtonState(XBOX_BUTTON_ID_A);
	if(xboxButtonStateA.WasJustPressed()){
		Fire();
	}
	float magnitude = 0.f;
	float turningDegrees = 0.f;
	float turnedDegrees = 0.f;
	m_deltaDegrees = 0.f;
	m_orientationDegrees = ClampDegressTo360(m_orientationDegrees);

	if(xboxController.isConnected()){
		//using xbox input
		AnalogJoystick leftJoyStick = xboxController.GetLeftJoystick();
		magnitude = leftJoyStick.GetMagnitude();
		if(magnitude > 0){
			turningDegrees = leftJoyStick.GetAngleDegrees();
			turningDegrees = ClampDegressTo360(turningDegrees);
		}
	}
	else{
		//using keyboard input
		if(g_theInputSystem->IsKeyDown(KEYBOARD_BUTTON_ID_E)){
			magnitude = 1.f;
			turningDegrees = 90.f;
		}	
		else{
			magnitude = 0.f;
			turningDegrees = 0.f;
		}
	}
	if(magnitude > 0){
		if( fabsf( turningDegrees-m_orientationDegrees ) > MAX_TURNING_DIFFERENCE ) {
			turnedDegrees = GetTurnedToward( m_orientationDegrees, turningDegrees, m_angularVelocity * deltaSeconds );
			m_deltaDegrees = turnedDegrees - m_orientationDegrees;
		}
		else{
			turnedDegrees = turningDegrees;
			m_deltaDegrees = 0.f;
		}

		m_orientationDegrees = turnedDegrees;
		
	}
	if(m_map->m_isPlayerInMud){
		magnitude /= 2;
	}
	m_velocity.SetPolarDegrees(m_orientationDegrees,magnitude * PLAYER_SPEED );

	//check if in lava
	if(m_map->m_isPlayerInLava){
		m_timeInLava += deltaSeconds;
		if(m_timeInLava > 1){
			TakeDamage();
			m_timeInLava = 0;
		}
	}
	else{
		m_timeInLava = 0;
	}
}

void Player::UpdateXboxVibration( float deltaSeconds )
{
	m_vibrationIntense -= m_vibrationDownRate * deltaSeconds;
	float tem = m_vibrationIntense;
	if( tem <= 0 ) {
		tem = 0;
	}
	const XboxController xboxController = g_theInputSystem->GetXboxController(0);
	xboxController.setVibration( tem );
}

void Player::UpdateBarrel( float deltaSeconds )
{
	const XboxController xboxController = g_theInputSystem->GetXboxController( 0 );
	float magnitude = 0.f;
	float turningDegrees = 0.f;
	float turnedDegrees = 0.f;
	m_barrelOrientationDegrees = ClampDegressTo360(m_barrelOrientationDegrees);

	if(xboxController.isConnected()){
		AnalogJoystick rightJoyStick = xboxController.GetRightJoystick();
		magnitude = rightJoyStick.GetMagnitude();
		if(magnitude > 0){
			turningDegrees = rightJoyStick.GetAngleDegrees();
			turningDegrees = ClampDegressTo360(turningDegrees);
		}
	}
	if(magnitude > 0 ){
		if( fabsf( turningDegrees-m_barrelOrientationDegrees ) > MAX_TURNING_DIFFERENCE ) {
			turnedDegrees = GetTurnedToward( m_barrelOrientationDegrees, turningDegrees, m_barrelAngularVelocity * deltaSeconds );
		}
		else {
			turnedDegrees = turningDegrees;
					
		}
		turnedDegrees = ClampDegressTo360(turnedDegrees);
		m_barrelOrientationDegrees = turnedDegrees;
	}
	m_barrelOrientationDegrees += m_deltaDegrees;
}

void Player::RenderTank() const
{
	const std::vector<Vertex_PCU> m_tankShapeInWorld=TransformVertexArray(m_tankVertices,1,m_orientationDegrees,m_position);
	
	g_theRenderer->BindTexture(m_tankTexture);
	g_theRenderer->DrawVertexVector(m_tankShapeInWorld);
}

void Player::RenderBarrel() const
{
	const std::vector<Vertex_PCU> m_barrelShapeInWorld=TransformVertexArray(m_barrelVertices,1,m_barrelOrientationDegrees,m_position);
	g_theRenderer->BindTexture( m_barrelTexture );
	g_theRenderer->DrawVertexVector( m_barrelShapeInWorld);

}

void Player::CreateTank()
{	//emplace_back(constructor arguments directly)
	m_tankShape = AABB2(Vec2(-.5f,-.5f),Vec2(.5f,.5f));
	m_tankVertices.push_back(Vertex_PCU(Vec3(m_tankShape.mins,0),Rgba8(255,255,255),Vec2(0,0)));
	m_tankVertices.push_back(Vertex_PCU(Vec3(m_tankShape.mins.x,m_tankShape.maxs.y,0),Rgba8(255,255,255),Vec2(0,1)));
	m_tankVertices.push_back(Vertex_PCU(Vec3(m_tankShape.maxs,0),Rgba8(255,255,255),Vec2(1,1)));
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs.x, m_tankShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

	m_barrelShape = AABB2(Vec2(-.5f,-.5f),Vec2(.5f,.5f));
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.mins.x, m_barrelShape.maxs.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 1 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.maxs.x, m_barrelShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

	m_tankTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
	m_barrelTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankTop.png");
	playerShoot = g_theAudioSystem->CreateOrGetSound("Data/Audio/PlayerShootNormal.ogg");
	playerHit = g_theAudioSystem->CreateOrGetSound("Data/Audio/PlayerHit.wav");
	playerDie = g_theAudioSystem->CreateOrGetSound("Data/Audio/PlayerDied.wav");

}

void Player::Fire()
{
	Vec2 fwdDir = Vec2(1.f,0.f);
	fwdDir.SetAngleDegrees(m_barrelOrientationDegrees);
	Vec2 spawnPos = m_position + fwdDir * 0.3f;
	m_map->SpawnNewBullet(m_faction,spawnPos,fwdDir);
	g_theAudioSystem->PlaySound(playerShoot);
}
