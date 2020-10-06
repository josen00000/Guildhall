#pragma once
#include "Socket.hpp"



class UDPSocket : protected Socket {
public:
	UDPSocket() = default;
	UDPSocket( UDPSocket const& copyFrom );
	~UDPSocket();

	virtual void CreateSocket( char const* portNum, const char* hostName = "" ) override;
	virtual void BindSocket( int port ) override;
	int UDPSend( int length );
	int UDPReceive();
	virtual void Close() override;

public:
	sockaddr_in m_bindAddr;
	sockaddr_in m_toAddr;
};