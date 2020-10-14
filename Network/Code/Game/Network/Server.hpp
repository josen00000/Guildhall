#pragma once

class Game;
class World;
class EntityFactory;

class Server {
	Server() = default;
	virtual ~Server();
	Server( Server const& copyFrom ) = delete;
	Server( Server const&& moveFrom ) = delete;

protected:
	Game*	m_game					= nullptr;
	World*	m_world					= nullptr;
	EntityFactory* m_entityFactory	= nullptr;
};