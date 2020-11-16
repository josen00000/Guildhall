#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
//#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Network/BlockingQueue.hpp"
#include "Engine/Network/NonBlockingQueue.hpp"
#include <thread>
#include <mutex>

class TCPClient;
class TCPServer;
class UDPSocket;

using Port			= std::string;
using IPAddress		= std::string;
using GameMessage	= std::string;

using GameInfo = std::pair<IPAddress, GameMessage>;

enum MESSAGE_ID: std::uint16_t {
	NON_VALID_DATA		= 0,
	SERVER_LISTENING,
	TEXT_MESSAGE,
	CLIENT_REQUEST_CONNECTION,
	SERVER_READY_FOR_CONNECTION,
	CLIENT_DISCONNECT,
	SERVER_DISCONNECT,
	NUM_OF_MESSAGE_ID
};

struct DataPackage {
	DataHeader header;
	char message[NET_MESSAGE_SIZE];
};

struct DataBuffer {
	char data[NET_BUFFER_SIZE];
};

class NetworkSystem
{
public:
	NetworkSystem();
	~NetworkSystem();

public:
	void StartUp();
	void BeginFrame();
	void Update( float deltaSeconds );
	void EndFrame();
	void Shutdown();
	void StartTCPServerWithPort( const char* port );

	void CloseUDPSockets();
	UDPSocket* CreateUDPSocket();
	void SetUDPSocketTargetPortAndIPAddress( UDPSocket* socket, Port port, IPAddress addr);
	void SetUDPSocketBindPort( UDPSocket* socket, Port port );

	// Accessor
	GameInfo GetReceivedUDPMsg();
	std::vector<GameInfo> GetAllReceivedUDPMsgs();
	std::string GetLocalIPAddress();
	TCPServer* GetTCPServer() { return m_TCPServer; }
	TCPClient* GetTCPClient(){ return m_TCPClient; }
	Port GetValidPort();
	UDPSocket* FindUDPSocketWithLocalPort( Port port );
	UDPSocket* FindUDPSocketWithTargetIPAddressAndPort( IPAddress addr );

	// mutator
	void SetSendUDPData( GameInfo info );


	// tcp client
	TCPServer*	CreateTCPServer();
	TCPClient* CreateTCPClient();
	void TCPConnectToServerWithIPAndPort( const char* hostName, const char* portNum );

	static void ReadFromGameAndSendUDPMessage();
	static void ReceiveUDPMessageAndWriteToGame();


public:
// 	std::vector<TCPClient*> m_clients;
// 	std::vector<TCPServer*> m_servers;
	// temp use
	TCPClient* m_TCPClient;
	TCPServer* m_TCPServer;
	std::vector<UDPSocket*> m_UDPSockets;
	BlockingQueue <GameInfo> m_sendQueue{};
	NonBlockingQueue <GameInfo> m_receiveQueue{};
	//UDPSocket* m_UDPSocket = nullptr;
	std::thread m_readFromGameAndSendThread;
	std::thread m_receiveAndSendToGameThread;

	std::atomic<bool> m_isEnd = false;
	bool m_isServer = false;
private:
	int m_listenPort = -1;
	bool m_isListening = true;
	//SOCKET* m_listenSocket;
};

