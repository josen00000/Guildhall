#pragma once
#include "Engine/Network/NetworkSystem.hpp"
#include <string>
#include <winsock2.h>

class TCPClient
{
public:
	TCPClient();
	~TCPClient();

public:
	void StartUp();
	void ShutDown();
	void BeginFrame();
	void Update();
	// Accessor
	std::string GetStringFromData();

	// Mutator
	void SetTargetIP( const std::string& targetIP );
	void SetTargetPort( const std::string& targetPort );
	void SetSelfIP( const std::string& selfIP );
	void SetSelfPort( const std::string& selfPort );
	void SetSendData( const char* data, int dataLen );

	void SendRequestConnectionMessageToServer();
	void ConnectToServerWithIPAndPort( const char* hostName, const char* portNum );
	void DisconnectServer();


private:
	void CreateSocket();
	void CreateSocket( const char* hostName, const char* portNum ); // should combine together
	void Connect();													// should combine together
	void SendDataToServer( MESSAGE_ID mid );
	void SendDisconnectMessageToServer();
	void ReceiveDataFromServer();
	void ReceiveData();
	void DisconnectAndCloseSocket();

	// buffer
	void WriteDataToRecvBuffer( DataPackage data );

public:
	std::string m_targetIPAddress = "";
	std::string m_targetport = "";
	std::string m_IPAddress = "";
	std::string m_port = "";

	DataBuffer m_sendBuf;
	DataBuffer m_recvBuf;
	int m_sendBufLen = 0;
	bool m_isSendBufDirty = false;
	bool m_isRecvBufDirty = false;

	SOCKET m_targetSocket = INVALID_SOCKET;
	FD_SET m_listenSet;
	timeval m_timeval = timeval{ 00, 05 };
	addrinfo* m_resultAddr = NULL;
	bool	m_isConnected = false;
	//std::string m_message = ""; // temp as buffer
};

