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

enum MESSAGE_ID: std::uint16_t {
	NON_VALID_DATA		= 0,
	SERVER_LISTENING,
	TEXT_MESSAGE,
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
	void StartServerWithPort( const char* port );

	void CloseUDPSocket();

	// Accessor
	std::string GetReceivedMsg();
	// mutator
	void SetSendMsg( std::string msg );


	TCPClient* CreateClient();
	TCPServer*	CreateServer();

	static void ReadFromGameAndSendMessage();
	static void ReceiveMessageAndWriteToGame();

public:
	std::vector<TCPClient*> m_clients;
	std::vector<TCPServer*> m_servers;
	// temp use
	TCPClient* m_client;
	TCPServer* m_server;
	BlockingQueue <std::string> m_sendQueue{};
	NonBlockingQueue <std::string> m_receiveQueue{};
	UDPSocket* m_UDPSocket = nullptr;
	std::thread m_readFromGameAndSendThread;
	std::thread m_receiveAndSendToGameThread;

	std::atomic<bool> m_isEnd = false;
	bool m_isServer = false;
private:
	int m_listenPort = -1;
	bool m_isListening = true;
	//SOCKET* m_listenSocket;
};

