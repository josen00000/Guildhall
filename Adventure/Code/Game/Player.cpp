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
	m_physicsRadius = 0.2f;
	m_cosmeticRadius=0.5f;
	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_health = 3; // should be 3
	CreatePlayer();
}

Player::~Player()
{
	delete m_PlayerTexture;
	m_PlayerTexture=nullptr;
}

void Player::Update( float deltaSeconds )
{
	UpdatePlayer( deltaSeconds );
	Entity::Update( deltaSeconds );
}

 void Player::Render()const
{
	RenderPlayer();
	if(g_theGame->m_developMode){
		Entity::Render();
	}
}

void Player::Die()
{
	//g_theAudioSystem->PlaySound(playerDie);
	if(m_map->m_world->m_playerLife == 0 ){
		m_map->m_world->m_playerLife--;
	}
	g_theGame->m_isPlayerDead = true;
	//m_map->m_playerDeadPos = m_position;
	Entity::Die();
}

void Player::TakeDamage()
{
	Entity::TakeDamage();
	if( m_health <= 0 ) {
		Die();
	}
}

void Player::CreatePlayer()
{	//emplace_back(constructor arguments directly)
	m_PlayerShape = AABB2(Vec2(-.5f,-.5f),Vec2(.5f,.5f));
	m_PlayerVertices.push_back(Vertex_PCU(Vec3(m_PlayerShape.mins,0),Rgba8(255,255,255),Vec2(0,0)));
	m_PlayerVertices.push_back(Vertex_PCU(Vec3(m_PlayerShape.mins.x,m_PlayerShape.maxs.y,0),Rgba8(255,255,255),Vec2(0,1)));
	m_PlayerVertices.push_back(Vertex_PCU(Vec3(m_PlayerShape.maxs,0),Rgba8(255,255,255),Vec2(1,1)));
	m_PlayerVertices.push_back( Vertex_PCU( Vec3( m_PlayerShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_PlayerVertices.push_back( Vertex_PCU( Vec3( m_PlayerShape.maxs.x, m_PlayerShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
	m_PlayerVertices.push_back( Vertex_PCU( Vec3( m_PlayerShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

	//m_PlayerTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
}

void Player::UpdatePlayer( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Player::RenderPlayer() const
{

}


