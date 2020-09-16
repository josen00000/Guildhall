#pragma once
#include "Engine/Network/NetworkSystem.hpp"
#include <WinSock2.h>
#include <vector>
#include <string>


class Server
{
public:
	Server();
	~Server();

public:
	void StartUp();
	void ShutDown();
	void Update();
	void BeginFrame();
	void PrepareForClientConnectionWithPort( const char* portNum );
	void CheckIfBufferDirty();

	// Mutator
	void SetSendData( const char* data, int dataLen );

	// Accessor
	std::string GetClientIPAddr( SOCKET clientSocket ); // only able after accept
	std::string GetStringFromData();
private:
	void ReceiveDataFromClient( SOCKET clientSocket );
	void SendDataToClient( SOCKET clientSocket, MESSAGE_ID mid );
	SOCKET CreateSocket( const char* portNum, addrinfo* socketAddr );
	void BindSocket( SOCKET socket, addrinfo* socketAddr );
	void ListenOnSocket( SOCKET socket );
	void AcceptConnectionFromClient( SOCKET listenSocket );
	void Disconnect();
	void CloseSocket( SOCKET socket );

private:
	addrinfo* m_resultAddr = nullptr;
	FD_SET m_listenSet;
	std::vector<SOCKET> m_listenSockets;
	std::vector<SOCKET> m_clientSockets;

	DataBuffer m_sendBuf;
	DataBuffer m_recvBuf;
	int	m_sendBufLen = 0;
	bool	m_isSendBufDirty = false;
	bool	m_isRecvBufDirty = false;
	//std::vector<>
	timeval m_timeval = timeval{ 01, 01 };
};

