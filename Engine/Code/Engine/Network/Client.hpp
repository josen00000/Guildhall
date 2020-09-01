#pragma once
#include <string>

class Client
{
public:
	Client();
	~Client();

public:
	void StartUp();
	void ShutDown();
	void CreateSocket();
	void Connect();
	void SendData();
	void ReceiveData();
	void DisConnect();

public:
	std::string m_targetIPAddress = "";
	std::string 
};

