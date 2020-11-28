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
struct ReliableUDPMsg;

using Port			= std::string;
using IPAddress		= std::string;
using GameMessage	= std::string;

//using GameInfo = std::pair<IPAddress, GameMessage>;
struct GameInfo {
public:
	GameInfo() = default;
	GameInfo( bool isReliable, std::string addr, std::string msg ) 
		:m_isReliable(isReliable)
		,m_addr(addr)
		,m_msg(msg)
	{
	}

	bool m_isReliable;
	std::string m_addr;
	std::string m_msg;
};

enum UDP_MSG_ID : std::uint16_t {
	NON_RELIABLE_MSG = 0,
	RELIABLE_MSG,
	NUM_OF_RELIABLE_MESSAGE_ID
};

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
	GameInfo GetGameInfoWithReliableUDPMessage( ReliableUDPMsg msg );
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
	static void ReceiveUDPMessageAndWriteToGame( UDPSocket* socket );
	static void WriteReliableUDPMessageToGame( UDPSocket* socket );
	static void SendReliableUDPMessage( UDPSocket* socket );


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

	std::atomic<bool> m_isEnd					= false;
	std::vector<std::thread> m_udpReceiveThreads;
	std::vector<std::thread> m_udpReliableSendThreads;
	std::vector<std::thread> m_udpReliableWriteToGameThreads;
	
	bool m_isServer = false;
private:
	int m_listenPort = -1;
	bool m_isListening = true;
	//SOCKET* m_listenSocket;
};

