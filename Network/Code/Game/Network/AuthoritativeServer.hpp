#pragma once
#include "Game/Network/Client.hpp"
#include "Game/Network/Server.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include <string>
#include <map>

class RandomNumberGenerator;

using Port = std::string;
using IPAddress = std::string;


class AuthoritativeServer : public Server {
public:
	AuthoritativeServer() = default;
	~AuthoritativeServer();
	explicit AuthoritativeServer( Game* game );
	AuthoritativeServer( AuthoritativeServer const& copyFrom ) = delete;
	AuthoritativeServer( AuthoritativeServer const&& moveFrom ) = delete;

	virtual void Startup() override;
	virtual void Shutdown() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void ReceiveAndHandleTCPNetworkData() override;
	virtual void ReceiveAndHandleUDPNetworkData() override;
	virtual void SendGameMsg( ) override;
	virtual void SendNetworkData() override ;
	virtual void ParseAndExecuteRemoteMsg( GameInfo msg ) override;
	// Accessor
	Client* FindClientWithIPAddress( IPAddress addr );
	Client* FindClientWithIPAddressAndPort( IPAddress addr );

	void CreateAndPushPlayer();
	void CreateAndPushRemoteClient( ClientInfo info );
	void CreateAndPushRemotePlayerEntityToClient( Client* client, Vec2 startPos );
	void MoveClientEntity( Client* client, std::string instruction );
	void PrepareConnectionForClientWithIPAddress( IPAddress addr );
	bool CheckIfClientApproved( IPAddress addr );
	void ReliableUDPMessageTest();


private:
	RandomNumberGenerator* m_rng;
};