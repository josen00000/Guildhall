#pragma once
#include "Game/Network/Client.hpp"

class RemoteClient : public Client {
public:
	RemoteClient() = default;
	~RemoteClient();
	explicit RemoteClient( Server* server );
	RemoteClient( RemoteClient const& copyFrom ) = delete;
	RemoteClient( RemoteClient const&& moveFrom ) = delete;


	virtual void Startup() override;
	virtual void Shutdown() override;
	virtual void BeginFrame() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void EndFrame() override;
	virtual void Render() override;
	virtual void HandleInput( std::string input ) override;
	virtual void SendDataToRemoteServer( Strings data ) override;
};