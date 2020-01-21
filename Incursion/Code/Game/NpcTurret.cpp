#include "NpcTurret.hpp"
#include "Game/App.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"

extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern Game* g_theGame;

NpcTurret::~NpcTurret()
{
	delete m_turretTexture;
	delete m_barrelTexture;
	m_turretTexture=nullptr;
	m_barrelTexture=nullptr;
}

NpcTurret::NpcTurret( Map* map, Vec2 iniPos, EntityType entityType, EntityFaction entityFaction )
	:Entity(map, iniPos, entityType, entityFaction)
{
	m_physicsRadius = 0.2f;
	m_cosmeticRadius=0.5f;
	CreateNpcTurret();
	m_health = TURRET_HEALTH;
	m_visionDist = TURRET_VISION_DISTANCE; 
	m_isPushedByWalls = true;
	m_isPushedByEntities = false;
	m_doesPushEntities = true;
	m_isHitByBullet = true;

}

void NpcTurret::Update( float deltaSeconds )
{
	switch( m_behavior )
	{
	case SEARCH_PLAYER:
	{
		if(m_targetPlayerLastDirection != Vec2::ZERO){
			PatrolForDirection(m_targetPlayerLastDirection, m_barrelAngularVelocity * deltaSeconds, 45);	
			if( m_isPlayerVisiable ) {
				m_behavior = ATTACT_PLAYER;
			}
			m_shootCoolDown = TURRET_MAX_SHOOT_DEGREES  ;
		}
		else{
			m_barrelOrientationDegrees += m_barrelAngularVelocity * deltaSeconds;
			if(m_isPlayerVisiable){
				m_behavior = ATTACT_PLAYER;
			}
			m_shootCoolDown = TURRET_MAX_SHOOT_DEGREES  ;
		}
		break;
	}
	case ATTACT_PLAYER:
	{
		m_shootCoolDown += deltaSeconds;
		m_targetDirection = m_targetPlayerPos - m_position;
		m_targetDirection.Normalize();
		float turningDegrees = m_targetDirection.GetAngleDegrees();
		turningDegrees = ClampDegressTo360(turningDegrees);
		if( fabsf( turningDegrees-m_barrelOrientationDegrees ) > TURRET_MAX_SHOOT_DEGREES ) {
			float turnedDegrees = GetTurnedToward(m_barrelOrientationDegrees, turningDegrees, m_barrelAngularVelocity * deltaSeconds);
			m_barrelOrientationDegrees = turnedDegrees;
		}
		else{
			if(m_shootCoolDown > TURRET_MAX_SHOOT_DEGREES){
				ShootPlayer();
			}
		}

		if(!m_isPlayerVisiable){
			m_targetPlayerLastDirection = m_targetDirection;
			m_behavior = SEARCH_PLAYER;
		}
		break;
	}
	default:
		break;
	}
	
	//Entity::Update(deltaSeconds);
}

void NpcTurret::Render() const
{
	RenderTurret();
	RenderBarrel();
	if( g_theGame->m_developMode ) {
		Entity::Render();
	}
}

void NpcTurret::Die()
{
	m_map->SpawnNewExplosion(m_position, TURRET_EXPLOSION_RADIU, TURRET_EXPLOSION_DURATION);
	g_theAudioSystem->PlaySound(npcTurretDie);
	Entity::Die();
}

void NpcTurret::TakeDamage()
{
	g_theAudioSystem->PlaySound(npcTurretHit);
	Entity::TakeDamage();
	if(m_health <=0){
		Die();
	}
}

void NpcTurret::ShootPlayer()
{
	g_theAudioSystem->PlaySound(npcTurretShoot);
	Vec2 spawnPos = m_position + m_targetDirection * 0.1f;
	m_map->SpawnNewBullet(m_faction,spawnPos,m_targetDirection);
	m_shootCoolDown = 0.f;
}

void NpcTurret::CreateNpcTurret()
{
	m_turretShape = AABB2( Vec2( -.5f, -.5f ), Vec2( .5f, .5f ) );
	m_turretVertices.push_back( Vertex_PCU( Vec3( m_turretShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_turretVertices.push_back( Vertex_PCU( Vec3( m_turretShape.mins.x, m_turretShape.maxs.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 1 ) ) );
	m_turretVertices.push_back( Vertex_PCU( Vec3( m_turretShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );
	m_turretVertices.push_back( Vertex_PCU( Vec3( m_turretShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_turretVertices.push_back( Vertex_PCU( Vec3( m_turretShape.maxs.x, m_turretShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
	m_turretVertices.push_back( Vertex_PCU( Vec3( m_turretShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

	m_barrelShape = AABB2( Vec2( -.5f, -.5f ), Vec2( .5f, .5f ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.mins.x, m_barrelShape.maxs.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 1 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.maxs.x, m_barrelShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

	m_turretTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTurretBase.png" );
	m_barrelTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTurretTop.png" );
	
	npcTurretShoot = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyShoot.wav" );
	npcTurretHit = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyHit.wav" );
	npcTurretDie = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyDied.wav" );
}





void NpcTurret::PatrolForDirection( Vec2 direction, float angularVelocityDegrees, float apertureDegrees )
{
	float targetDegrees = direction.GetAngleDegrees();
	
	targetDegrees = ClampDegressTo360( targetDegrees );
	//float test = fabsf(targetDegrees - m_barrelOrientationDegrees);
	if(fabsf(targetDegrees - m_barrelOrientationDegrees) > apertureDegrees ) {
		m_patrolCoefficient *= -1;
	}
	m_barrelOrientationDegrees += m_patrolCoefficient * angularVelocityDegrees;
}

void NpcTurret::RenderTurret() const
{
	const std::vector<Vertex_PCU> m_turretShapeInWorld=TransformVertexArray( m_turretVertices, 1, m_orientationDegrees, m_position );

	g_theRenderer->BindTexture( m_turretTexture );
	g_theRenderer->DrawVertexVector( m_turretShapeInWorld );
}

void NpcTurret::RenderBarrel() const
{
	const std::vector<Vertex_PCU> m_barrelShapeInWorld=TransformVertexArray( m_barrelVertices, 1, m_barrelOrientationDegrees, m_position );
	g_theRenderer->BindTexture( m_barrelTexture );
	g_theRenderer->DrawVertexVector( m_barrelShapeInWorld );
}
