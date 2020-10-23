#pragma once
#include "Game/Network/Server.hpp"

class RemoteServer : public Server {
public:
	RemoteServer() = default;
	~RemoteServer() = default;
	RemoteServer( RemoteServer const& copyFrom ) = delete;
	RemoteServer( RemoteServer const&& moveFrom ) = delete;

};