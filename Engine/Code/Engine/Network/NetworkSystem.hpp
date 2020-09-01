#pragma once
#include <vector>

class Client;
class Server;

class NetworkSystem
{
public:
	NetworkSystem();
	~NetworkSystem();

public:
	void StartUp();
	void BeginFrame();
	void Update();
	void EndFrame();
	void ShutDown();

	Client* createClient();
	Server*	CreateServer();

public:
	std::vector<Client*> m_clients;
	std::vector<Server*> m_servers;
};

