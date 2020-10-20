#include "Server.hpp"
#include "Game/Game.hpp"
#include "Game/Actor.hpp"
#include "Game/Network/Client.hpp"
#include "Game/Network/PlayerClient.hpp"
#include "Game/EntityFactory.hpp"
#include "Engine/Renderer/Camera.hpp"

extern Camera* g_camera;

Server::Server( Game* game )
{
	m_game = game;
}

Server::~Server()
{
}

void Server::Startup()
{
	m_entityFactory = new EntityFactory();
	Client* tempClient = new PlayerClient( this, g_camera );
	m_clients.push_back( tempClient );
	//m_game = g_theGame;
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
	m_game->Update( deltaSeconds );
	for( int i = 0; i < m_entities.size(); i++ ) {
		m_entities[i]->Update( deltaSeconds );
	}
}

void Server::BeginFrame()
{
	for( int i = 0; i < m_clients.size(); i++ ) {
		m_clients[i]->BeginFrame();
	}
}

void Server::EndFrame()
{

}

Entity* Server::CreateAndPushPlayer()
{
	Entity* player = m_entityFactory->CreateEntityWithDefinition( std::string( "player" ) );
	player->SetIsPlayer( true );
	m_entities.push_back( player );
	return player;
}

Entity* Server::CreateAndPushEntity( std::string entityType )
{
	Entity* entity = m_entityFactory->CreateEntityWithDefinition( entityType );
	m_entities.push_back( entity );
}

Entity* Server::SpawnNewEntityOfType( EntityDefinition const& entityDef )
{
	Entity* result = nullptr;
	if( entityDef.m_type.compare( "Actor" ) == 0 ) {
		result = new Actor( entityDef );
	}
// 	else if( entityDef.m_type.compare( "Projectile" ) == 0 ) {
// 		result = new Projectile();
// 	}
// 	else if( entityDef.m_type.compare( "Portal" ) == 0 ) {
// 		result = new Portal( entityDef );
// 	}
	else if( entityDef.m_type.compare( "Entity" ) == 0 ) {
		result = new Entity( entityDef );
	}
	return result;
}

Convention Server::GetGameConvention() const
{
	return m_game->m_convension;
}

