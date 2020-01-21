#include "NpcTank.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"


extern RenderContext* g_theRenderer;
extern InputSystem* g_theInputSystem;
extern AudioSystem* g_theAudioSystem;
extern Game* g_theGame;

NpcTank::~NpcTank()
{
	delete m_tankTexture;
	m_tankTexture = nullptr;
}

NpcTank::NpcTank( Map* map ,Vec2 spawnPos)
	:Entity(map,spawnPos ,ENTITY_TYPE_NPC_TANK, ENTITY_FACTION_EVIL)
{
	m_physicsRadius = 0.2f;
	m_cosmeticRadius=0.5f;
	CreateTank();
	m_health = ENEMY_TANK_HEALTH;
	m_visionDist = ENEMY_TANK_VISION_DISTANCE;
	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullet = true;

}

void NpcTank::Update( float deltaSeconds )
{
	
	Vec2 fwdDir = Vec2 (1.f , 0.f );
	UpdateRaycastResult();
	switch( m_behavior )
	{
	case TANK_SEARCH_PLAYER:
	{
		if(m_targetLastPos != Vec2::ZERO){
			//move to last seen pos
			m_targetDirection = m_targetLastPos - m_position;
			if(IsPointInForwardSector2D(m_targetLastPos,m_position,m_orientationDegrees, ENEMY_TANK_MAX_SHOOT_DEGREES,100 )){
				fwdDir.SetAngleDegrees(m_orientationDegrees);
				m_velocity = fwdDir * ENEMY_TANK_SPEED;
			}
			else{
				float targetDegrees = m_targetDirection.GetAngleDegrees();
				float turnedDegrees = GetTurnedToward(m_orientationDegrees,targetDegrees, ENEMY_TANK_ANGULAR_VELOCITY * deltaSeconds);
				m_orientationDegrees = turnedDegrees;
				m_velocity = Vec2::ZERO;
			}
			float dist = m_targetDirection.GetLength();
			if(dist < 0.5){
				m_targetLastPos = Vec2::ZERO;
			}
		}
		else{
			// normal wander
			// check collision with wall 
			if(m_isEscapeFromWall){
				m_escapeTime += deltaSeconds;
				if( !IsTowardsToTargetDegrees( m_targetDegrees, ENEMY_TANK_MAX_SHOOT_DEGREES ) ) {
					RotateToTargetDegrees( m_targetDegrees, ENEMY_TANK_ANGULAR_VELOCITY * deltaSeconds, false );
					if(!CheckCollisionWithWall()){
						m_targetDegrees = m_orientationDegrees;
					}
				}
				else {
					fwdDir.SetAngleDegrees( m_orientationDegrees );
					m_velocity = fwdDir * ENEMY_TANK_SPEED;
					m_escapeDist -= ENEMY_TANK_SPEED * deltaSeconds;
					if( m_escapeDist < 0 ) {
						m_isEscapeFromWall = false;
					}
				}
				if(m_escapeTime > 0.5){
					bool isCollide = CheckCollisionWithWall();
					if( isCollide ) {
						handleCollisionWithWall();
					}
				}
			}
			else{
				// normal wandering
				
				bool isCollide = CheckCollisionWithWall();
				if(isCollide){
					handleCollisionWithWall();
				}
				else{
					if( m_moveCoolDown >= ENEMY_TANK_MOVE_COOL_DOWN ) {
						m_targetDegrees = m_rmg.RollRandomFloatInRange( 0, 360 );
						m_moveCoolDown = 0.f;
					}

					if( fabsf( m_targetDegrees - m_orientationDegrees ) > ENEMY_TANK_MAX_SHOOT_DEGREES ) {
						float turnedDegrees = GetTurnedToward( m_orientationDegrees, m_targetDegrees, ENEMY_TANK_ANGULAR_VELOCITY * deltaSeconds );
						m_orientationDegrees = turnedDegrees;
						m_velocity = Vec2::ZERO;
						m_moveCoolDown = 0.f;
					}
					else {
						fwdDir.SetAngleDegrees( m_orientationDegrees );
						m_velocity = fwdDir * ENEMY_TANK_SPEED;
						m_moveCoolDown += deltaSeconds;

					}
				}
			}
			
		}


		if( m_isPlayerVisiable ) {
			m_behavior = TAHK_ATTACT_PLAYER;
		}

		m_shootCoolDown = ENEMY_TANK_MAX_SHOOT_DEGREES;
		break;
	}
	case TAHK_ATTACT_PLAYER:
	{
		m_shootCoolDown += deltaSeconds;
		m_targetDirection = m_targetPlayerPos - m_position;
		m_targetDirection.Normalize();
		float turningDegrees = m_targetDirection.GetAngleDegrees();
		turningDegrees = ClampDegressTo360( turningDegrees );
		if( fabsf( turningDegrees - m_orientationDegrees ) > ENEMY_TANK_MAX_MOVE_DEGREES ) {
			float turnedDegrees = GetTurnedToward( m_orientationDegrees, turningDegrees, ENEMY_TANK_ANGULAR_VELOCITY * deltaSeconds );
			m_orientationDegrees = turnedDegrees;
			m_velocity = Vec2::ZERO;
		}
		else if( fabsf( turningDegrees - m_orientationDegrees ) > ENEMY_TANK_MAX_SHOOT_DEGREES ) {
			float turnedDegrees = GetTurnedToward( m_orientationDegrees, turningDegrees, ENEMY_TANK_ANGULAR_VELOCITY * deltaSeconds );
			m_orientationDegrees = turnedDegrees;
			fwdDir.SetAngleDegrees( m_orientationDegrees );
			m_velocity = fwdDir * ENEMY_TANK_SPEED;
		}
		else {
			//m_velocity = Vec2::ZERO;
			if( m_shootCoolDown > ENEMY_TANK_MAX_SHOOT_DEGREES ) {
				Fire();
			}
		}

		if( !m_isPlayerVisiable ) {
			m_targetLastPos = m_targetPlayerPos;
			m_behavior = TANK_SEARCH_PLAYER;
		}
		break;
	}
	default:
		break;
	}

	

	Entity::Update(deltaSeconds);
}

void NpcTank::Render() const
{
	g_theRenderer->BindTexture(nullptr);
	if( g_theGame->m_developMode ) {
		if(m_targetLastPos != Vec2::ZERO){
			g_theRenderer->DrawLine(m_position,m_targetLastPos,LINE_THICK, Rgba8::RED);
			g_theRenderer->DrawCircle(m_targetLastPos,0.1f,LINE_THICK,Rgba8::RED);
		}
			g_theRenderer->DrawLine(leftRaycastResult.m_start, leftRaycastResult.m_impactPos, LINE_THICK,Rgba8::BLUE);
			g_theRenderer->DrawLine(midRaycastResult.m_start, midRaycastResult.m_impactPos, LINE_THICK,Rgba8::BLUE);
			g_theRenderer->DrawLine(rightRaycastResult.m_start, rightRaycastResult.m_impactPos, LINE_THICK,Rgba8::BLUE);
		Entity::Render();
	}
	RenderTank();
}

void NpcTank::Die()
{
	m_map->SpawnNewExplosion(m_position,ENEMY_TANK_EXPLOSION_RADIU, ENEMY_TANK_EXPLOSION_DURATION);
	g_theAudioSystem->PlaySound(npcTankDie);
	Entity::Die();
}

void NpcTank::TakeDamage()
{
	g_theAudioSystem->PlaySound(npcTankHit);
	Entity::TakeDamage();
	if( m_health <= 0 ) {
		Die();
	}
}




void NpcTank::RenderTank() const
{
	const std::vector<Vertex_PCU> m_tankShapeInWorld=TransformVertexArray( m_tankVertices, 1, m_orientationDegrees, m_position );

	g_theRenderer->BindTexture( m_tankTexture );
	g_theRenderer->DrawVertexVector( m_tankShapeInWorld );
}


void NpcTank::CreateTank()
{

	m_tankShape = AABB2( Vec2( -.5f, -.5f ), Vec2( .5f, .5f ) );
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins.x, m_tankShape.maxs.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 1 ) ) );
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs.x, m_tankShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

	m_tankTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTank1.png" );
	
	npcTankShoot = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyShoot.wav" );
	npcTankHit = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyHit.wav" );
	npcTankDie = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyDied.wav" );
}

bool NpcTank::IsTowardsToTargetDegrees( float targetDegrees, float apertureDegrees )
{
	if(fabsf( targetDegrees - m_orientationDegrees ) < apertureDegrees){
		return true;

	}
	else{
		return false;
	}
}






void NpcTank::RotateToTargetDegrees( float targetDegrees, float angularSpeedsDegrees, bool ableMove )
{
	float turnedDegrees = GetTurnedToward( m_orientationDegrees, targetDegrees, angularSpeedsDegrees );
	m_orientationDegrees = turnedDegrees;
	Vec2 fwdDir;
	fwdDir.SetAngleDegrees( m_orientationDegrees );
	if(ableMove){
		m_velocity = fwdDir * ENEMY_TANK_SPEED;
	}
	else{
		m_velocity= Vec2::ZERO;
	}
}

void NpcTank::UpdateRaycastResult()
{
	Vec2 fwdDir;
	Vec2 leftFwdDir;
	Vec2 rightFwdDir;
	float leftRaycastDegrees = m_orientationDegrees + 10.f;
	float rightRaycastDegrees = m_orientationDegrees - 10.f;
	fwdDir.SetPolarDegrees( m_orientationDegrees, 1 );
	leftFwdDir.SetPolarDegrees( leftRaycastDegrees, 1 );
	rightFwdDir.SetPolarDegrees( rightRaycastDegrees, 1 );
	Vec2 perpendFwdDir = fwdDir;
	perpendFwdDir.Rotate90Degrees();
	Vec2 leftPos = m_position + perpendFwdDir * 0.3f;
	Vec2 rightPos = m_position - perpendFwdDir * 0.3f;
	midRaycastResult = m_map->Raycast( m_position, fwdDir, ENEMY_TANK_RAYCAST_DIST );
	leftRaycastResult = m_map->Raycast( leftPos, leftFwdDir, ENEMY_TANK_RAYCAST_DIST );
	rightRaycastResult = m_map->Raycast( rightPos, rightFwdDir, ENEMY_TANK_RAYCAST_DIST );
}

bool NpcTank::CheckCollisionWithWall()
{
	if(leftRaycastResult.m_didImpact || rightRaycastResult.m_didImpact || midRaycastResult.m_didImpact){
		return true;
	}
	else{
		return false;
	}
}

void NpcTank::handleCollisionWithWall()
{
	m_isEscapeFromWall = true;
	if(leftRaycastResult.m_didImpact && rightRaycastResult.m_didImpact && midRaycastResult.m_didImpact){
		m_escapeDegree = 180;
		m_escapeDist = 1;
	}
	if(!midRaycastResult.m_didImpact){
		if(leftRaycastResult.m_didImpact && leftRaycastResult.m_impactDist < rightRaycastResult.m_impactDist){
			m_escapeDegree = -90;
			m_escapeDist = 1;
		}
		else if( rightRaycastResult.m_didImpact && leftRaycastResult.m_impactDist > rightRaycastResult.m_impactDist ) {
			m_escapeDegree = 90;
			m_escapeDist = 1;
		}
	}
	m_targetDegrees = m_orientationDegrees + m_escapeDegree;
	m_targetDegrees = ClampDegressTo360(m_targetDegrees);
	m_escapeTime = 0.f;
}

void NpcTank::Fire()
{
	Vec2 spawnPos = m_position + m_targetDirection * 0.1f;
	m_map->SpawnNewBullet(m_faction,spawnPos,m_targetDirection);
	m_shootCoolDown = 0.f;
	g_theAudioSystem->PlaySound(npcTankShoot);
}
