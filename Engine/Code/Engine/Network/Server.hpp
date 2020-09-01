#pragma once

class Server
{
public:
	Server();
	~Server();

public:
	void StartUp();
	void ShutDown();
	void Update();

	void CreateSocket();
	void BindSocket();
	void ListenOnSocket();
	void AcceptConnectionFromClient();
	void ReceiveData();
	void SendData();
	void Disconnect();

};

