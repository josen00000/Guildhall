#pragma once
#include "Game/Network/Server.hpp"

class AuthoritativeServer : public Server {
	AuthoritativeServer() = default;
	~AuthoritativeServer();
	AuthoritativeServer( AuthoritativeServer const& copyFrom ) = delete;
	AuthoritativeServer( AuthoritativeServer const&& moveFrom ) = delete;

};