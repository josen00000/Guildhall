#pragma once
#include "Game/Network/Server.hpp"

class RemoteServer : public Server {
public:
	RemoteServer() = default;
	~RemoteServer() = default;
	explicit RemoteServer( Game* game );
	RemoteServer( RemoteServer const& copyFrom ) = delete;
	RemoteServer( RemoteServer const&& moveFrom ) = delete;


	virtual void Startup() override;
	virtual void Shutdown() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void ReceiveAndHandleTCPNetworkData() override;
	virtual void ReceiveAndHandleUDPNetworkData() override;
	virtual void ParseAndExecuteRemoteMsg( GameInfo msg ) override;
	virtual void SendNetworkData() override ;

	// Accessor
	bool GetIsConnectedFinished() const { return m_isConnectFinished; }

	// Mutator
	void SetIsConnectedFinished( bool isConnectFinished );
	void SetTargetIPAddress( std::string targetAddr );

	void SendTCPConnectionRequest();
	void SendUDPConnectionRequest();
private:
	int m_playerIndex = 0;
	bool m_isConnectFinished = false;
	bool m_doesSendTCPRequest = false;
	std::string m_key;
	std::string m_targetPort;
	std::string m_targetIPAddr;
};