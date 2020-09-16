#pragma once
#include <string>
#include <winsock2.h>

class Client
{
public:
	Client();
	~Client();

public:
	void StartUp();
	void ShutDown();

	// Accessor
	
	// Mutator
	void SetTargetIP( const std::string& targetIP );
	void SetTargetPort( const std::string& targetPort );
	void SetSelfIP( const std::string& selfIP );
	void SetSelfPort( const std::string& selfPort );
	void SendMessage( std::string message );


	void CreateSocket();
	void CreateSocket( const char* hostName, const char* portNum ); // should combine together
	void Connect();													// should combine together
	void SendData();
	void ReceiveData();
	void DisConnect();

public:
	std::string m_targetIPAddress = "";
	std::string m_targetport = "";
	std::string m_IPAddress = "";
	std::string m_port = "";

	SOCKET m_socket = INVALID_SOCKET;
	addrinfo* m_resultAddr = NULL;
	bool	m_isConnected = false;
	std::string m_message = ""; // temp as buffer
};

