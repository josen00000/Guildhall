#pragma once
#include <WinSock2.h>
//#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

struct SocketBuffer {
	DataHeader header;
	char data[NET_MESSAGE_SIZE];
};

class UDPSocket {
public:
	UDPSocket() = default;
	~UDPSocket(){}

	void CreateUDPSocket( char const* portNum, const char* hostName = "" );
	void BindSocket( int port ) ;
	void SetHeader( std::uint16_t id, std::uint16_t length, std::uint32_t seq );
	void SetHeader( DataHeader header );
	void WriteData( char const* dataPtr, int dataLen );
	std::string ReadDataAsString();
	std::string GetReceiveIPAddr() const { return m_fromAddrText; }
	DataHeader* GetReceiveHeader() { return &(m_receiveBuffer.header); }
	int UDPSend( int length );
	int UDPReceive();
	void Close();

public:
	SOCKET		m_socket = INVALID_SOCKET;
	addrinfo*	m_addrInfo = nullptr;
	SocketBuffer m_sendBuffer;
	SocketBuffer m_receiveBuffer;
	sockaddr_in m_bindAddr;
	sockaddr_in m_toAddr;
	std::string m_fromAddrText;
};