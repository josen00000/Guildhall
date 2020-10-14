#pragma once
#include "Game/Network/Client.hpp"

class PlayerClient : public Client {
	PlayerClient() = default;
	~PlayerClient();
	PlayerClient( PlayerClient const& copyFrom ) = delete;
	PlayerClient( PlayerClient const&& moveFrom ) = delete;

};