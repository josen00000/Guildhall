#include "Server.hpp"
#include "Game/Game.hpp"
#include "Game/Actor.hpp"
#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Game/Network/Client.hpp"
#include "Game/Network/PlayerClient.hpp"
#include "Game/EntityFactory.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Network/NetworkSystem.hpp"

extern Camera*			g_camera;
extern Camera*			g_UICamera;
extern Game*			g_theGame;
extern NetworkSystem*	g_theNetworkSystem;


Server::Server( Game* game )
{
	m_game = game;
}

Server::~Server()
{
	for( int i = 0; i < m_entities.size(); i++ ) {
		delete m_entities[i];
	}
	
	SELF_SAFE_RELEASE( m_entityFactory );
	SELF_SAFE_RELEASE( m_world );
	SELF_SAFE_RELEASE( m_game );
	SELF_SAFE_RELEASE( m_playerClient );
}


void Server::Startup()
{
	// create player client and entity factory
	m_entityFactory = new EntityFactory();
	m_playerClient = new PlayerClient( this, g_camera ); 
	m_playerClient->SetUICamera( g_UICamera );
	m_playerClient->Startup();
	m_game = g_theGame;


}

void Server::Shutdown()
{
	m_game->Shutdown();	
}

void Server::Update( float deltaSeconds )
{
	for( int i = 0; i < m_clients.size(); i++ ) {
		m_clients[i]->Update( deltaSeconds );
	}
	m_playerClient->Update( deltaSeconds );
	m_game->Update( deltaSeconds );
	for( int i = 0; i < m_entities.size(); i++ ) {
		m_entities[i]->Update( deltaSeconds );
	}
	Map* currentMap = m_game->m_world->GetCurrentMap();
	currentMap->CheckCollision();
}

void Server::BeginFrame()
{
	for( int i = 0; i < m_clients.size(); i++ ) {
		m_clients[i]->BeginFrame();
	}
	m_playerClient->BeginFrame();
	ReceiveAndHandleNetworkData();
}

void Server::EndFrame()
{
	SendNetworkData();
	for( int i = 0; i < m_clients.size(); i++ ) {
		m_clients[i]->EndFrame();
	}
	m_playerClient->EndFrame();
}

void Server::ReceiveAndHandleNetworkData()
{
	ReceiveAndHandleTCPNetworkData();
	ReceiveAndHandleUDPNetworkData();
}

void Server::ReceiveAndHandleTCPNetworkData()
{
	
}

void Server::ReceiveAndHandleUDPNetworkData()
{

}

void Server::SendNetworkData()
{

}

void Server::SendGameMsg( )
{

}

Entity* Server::CreateAndPushPlayer()
{
	Entity* player = m_entityFactory->CreateEntityWithDefinition( std::string( "Marine" ) );
	player->SetIsPlayer( true );
	player->Set2DPos( Vec2( 1.5f, 1.5f ) );
	m_entities.push_back( player );
	return player;
}

Entity* Server::CreateAndPushEntityWithPos( EntityType entityType, Vec2 pos )
{
	Entity* tempEntity = m_entityFactory->SpawnNewEntityOfType( entityType );
	tempEntity->Set2DPos( pos );
	m_entities.push_back( tempEntity );
	return tempEntity;
}

Convention Server::GetGameConvention() const
{
	return m_game->m_convension;
}

std::vector<Entity*> Server::GetEntities()
{
	return m_entities;
}

