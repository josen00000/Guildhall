#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"

class Client;
class Server;

enum MESSAGE_ID: std::uint16_t {
	SERVER_LISTENING	= 1,
	TEXT_MESSAGE,
	CLIENT_DISCONNECTING,
	NUM_OF_MESSAGE_ID_PLUS_1
};

struct DataHeader {
	std::uint16_t messageID = 0;
	std::uint16_t messageLen = 0;
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

	Client* CreateClient();
	Server*	CreateServer();

public:
	std::vector<Client*> m_clients;
	std::vector<Server*> m_servers;
	// temp use
	Client* m_client;
	Server* m_server;

private:
	int m_listenPort = -1;
	bool m_isListening = true;
	//SOCKET* m_listenSocket;
};

