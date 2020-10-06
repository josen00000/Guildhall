#include "UDPSocket.hpp"

extern DevConsole* g_theConsole;
#define LOG_ERROR(...) g_theConsole->DebugErrorf(__VA_ARGS__)

UDPSocket::~UDPSocket()
{
	Close();
}

void UDPSocket::CreateSocket( char const* portNum, const char* hostName /*= "" */ )
{
	UNUSED(hostName);
	addrinfo addrHints;
	ZeroMemory( &addrHints, sizeof( addrHints ) );

	memset( &m_toAddr, 0, sizeof(m_toAddr) );
	m_toAddr.sin_family = AF_INET;
	u_short iPortNum			= (u_short)atoi(portNum);
	m_toAddr.sin_port			= htons(iPortNum);
	m_toAddr.sin_addr.s_addr	= INADDR_ANY;

	m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( m_socket == INVALID_SOCKET ) {
		LOG_ERROR( "Invalid socket" );
	}
}

void UDPSocket::BindSocket( int port )
{
	m_bindAddr.sin_family		= AF_INET;
	m_bindAddr.sin_port			= htons((u_short)port);
	m_bindAddr.sin_addr.s_addr	= INADDR_ANY;

	int result = bind( m_socket, reinterpret_cast<sockaddr*>(&m_bindAddr), sizeof(m_bindAddr) );
	if( result == SOCKET_ERROR ) {
		LOG_ERROR( "socket bind failed, error: %d", WSAGetLastError() );
	}
}

int UDPSocket::UDPSend( int length )
{
	int result = sendto( m_socket, &m_sendBuffer.data[0], length, 0, reinterpret_cast<sockaddr*>(&m_toAddr), sizeof(m_toAddr) );
	if( result == SOCKET_ERROR ) {
		LOG_ERROR("Socket send error: %d", WSAGetLastError() );
	}
	return result;
}

int UDPSocket::UDPReceive()
{
	sockaddr_in fromAddr;
	memset( &fromAddr, 0, sizeof(fromAddr) );
	int fromLen = sizeof(fromAddr);
	int result = recvfrom( m_socket, &m_receiveBuffer.data[0], NET_BUFFER_SIZE, 0, reinterpret_cast<sockaddr*>(&fromAddr), &fromLen );
	if( result == SOCKET_ERROR ) {
		LOG_ERROR("Socket recv error: %d", WSAGetLastError() );
	}
	return result;
}

void UDPSocket::Close()
{
	if( m_socket != INVALID_SOCKET ) {
		int result = closesocket( m_socket );
		if( result == SOCKET_ERROR ) {
			LOG_ERROR("Socket close error: %d", WSAGetLastError() );
		}
		m_socket = INVALID_SOCKET;
	}
	memset( &m_bindAddr, 0, sizeof(m_bindAddr) );
	memset( &m_toAddr, 0, sizeof(m_toAddr) );

}

