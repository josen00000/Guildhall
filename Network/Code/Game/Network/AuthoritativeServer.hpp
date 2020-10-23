#pragma once
#include "Game/Network/Server.hpp"

class AuthoritativeServer : public Server {
public:
	AuthoritativeServer() = default;
	~AuthoritativeServer() = default;
	explicit AuthoritativeServer( Game* game );
	AuthoritativeServer( AuthoritativeServer const& copyFrom ) = delete;
	AuthoritativeServer( AuthoritativeServer const&& moveFrom ) = delete;

	void CreateAndPushPlayer();
};