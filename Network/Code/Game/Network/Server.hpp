#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"

class Client;
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
	Entity* CreateAndPushPlayer();
	Entity* CreateAndPushEntity( std::string entityType );
	Entity* SpawnNewEntityOfType( EntityDefinition const& entityDef );

	// Accessor
	Convention GetGameConvention() const;

protected:
	Game*	m_game					= nullptr;
	World*	m_world					= nullptr;
	EntityFactory* m_entityFactory	= nullptr;
	std::vector<Entity*> m_entities;
	std::vector<Client*> m_clients;
};