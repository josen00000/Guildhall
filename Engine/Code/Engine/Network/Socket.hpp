#pragma once

#include <winsock2.h>
#include "Engine/Network/NetworkSystem.hpp"


// struct SocketBuffer {
// 	DataHeader header;
// 	char data[NET_MESSAGE_SIZE];
// };

/*class Socket {
public:
	Socket() = default;
	virtual ~Socket();

	virtual void CreateSocket( char const* portNum, const char* hostName = "" );
	virtual void BindSocket( int port );
	virtual void Send( int length );
	virtual void Receive();
	virtual void Close();

public:
	SOCKET			m_socket	= INVALID_SOCKET;
	addrinfo*		m_addrInfo	= nullptr;
	SocketBuffer	m_sendBuffer;
	SocketBuffer	m_receiveBuffer;
};*/