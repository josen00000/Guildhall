#pragma once
#include <string>
#include "Engine/Core/StringUtils.hpp"

struct ClientInfo {
	std::string key			= "";
	std::string IPAdress	= "";
	std::string message		= "";
	std::string localPort	= "";
	bool isConnected		= false;
	bool isDataReadyForSend = false;
	char dataBuf[512];
	int	 dataLen			= 0;
};


class Server;
class Entity;


class Client {
public:
	Client() = delete;
	virtual ~Client();
	explicit Client( Server* owner );
	Client( Client const& copyFrom ) = delete;
	Client( Client const&& moveFrom ) = delete;

	// Accessor
	bool GetIsConnected() const { return m_clientInfo.isConnected; }
	bool GetIsDataReadyForSend() const { return m_clientInfo.isDataReadyForSend; }
	std::string GetKey() const { return m_clientInfo.key; }
	std::string GetIPAddress() const { return m_clientInfo.IPAdress; }
	std::string GetLocalPort() const { return m_clientInfo.localPort; }
	
	// Mutator
	void SetEntity( Entity* entity ); 
	void SetKey( std::string key );
	void SetIPAddress( std::string addr );
	void SetIsConnected( bool isConnected );
	void SetIsDataReadyForSend( bool isDataReady );
	void SetSendMessage( std::string msg );
	void SetClientInfo( ClientInfo info );


	virtual void Startup();
	virtual void Shutdown();
	virtual void BeginFrame();
	virtual void Update( float deltaSeconds );
	virtual void EndFrame();
	virtual void Render();
	virtual void HandleInput( std::string input ) = 0;
	virtual void SendDataToRemoteServer( Strings data );
	virtual void SendDataToRemoteServer( std::string data );
	virtual void SendReliableDataToRemoteServer( std::string data );
	
	Server* m_owner		= nullptr;
	Entity* m_entity	= nullptr;
	ClientInfo m_clientInfo;
};