#pragma once
#include "Game/Network/Client.hpp"

class RemoteClient : public Client {
	RemoteClient() = default;
	~RemoteClient();
	RemoteClient( RemoteClient const& copyFrom ) = delete;
	RemoteClient( RemoteClient const&& moveFrom ) = delete;

};