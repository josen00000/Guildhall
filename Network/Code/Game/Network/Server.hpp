#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Game/EntityDefinition.hpp"
#include "Engine/Network/NetworkSystem.hpp"

class Client;
class PlayerClient;
class Game;
class World;
class EntityFactory;
class Entity;


class Server {
public:
	Server() = default;
	virtual ~Server();
	Server( Server const& copyFrom ) = delete;
	Server( Server const&& moveFrom ) = delete;
	explicit Server( Game* game );

	virtual void Startup();  
	virtual void Shutdown();
	virtual void Update( float deltaSeconds );
	virtual void BeginFrame();
	virtual void EndFrame();
	virtual void ReceiveAndHandleNetworkData();
	virtual void ReceiveAndHandleTCPNetworkData();
	virtual void ReceiveAndHandleUDPNetworkData();
	virtual void SendNetworkData();
	virtual void SendGameMsg();
	virtual void ParseAndExecuteRemoteMsg( GameInfo msg ) = 0;
	Entity* CreateAndPushPlayer();
	Entity* CreateAndPushEntityWithPos( EntityType entityType, Vec2 pos );

	// Accessor
	Convention GetGameConvention() const;
	std::vector<Entity*> GetEntities();

public:
	Game*	m_game					= nullptr;
	World*	m_world					= nullptr;
	EntityFactory* m_entityFactory	= nullptr;
	std::vector<Entity*> m_entities;
	std::vector<Client*> m_clients;
	PlayerClient* m_playerClient	= nullptr;
	char m_UDPDataBuffer[10000];
	std::string m_TCPMsgBuffer;
};