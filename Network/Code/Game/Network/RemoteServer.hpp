#pragma once
#include "Game/Network/Server.hpp"

class RemoteServer : public Server {
	RemoteServer() = default;
	~RemoteServer();
	RemoteServer( RemoteServer const& copyFrom ) = delete;
	RemoteServer( RemoteServer const&& moveFrom ) = delete;

};