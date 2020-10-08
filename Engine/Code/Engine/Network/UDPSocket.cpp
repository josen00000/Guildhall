#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "UDPSocket.hpp"


// #ifdef TEST_MODE
 	#define LOG_ERROR(...) printf( __VA_ARGS__ )	
// #else
	//#define LOG_ERROR(...) g_theConsole->DebugErrorf(__VA_ARGS__)
//#endif // TEST_MODE

extern DevConsole* g_theConsole;

// UDPSocket::~UDPSocket()
// {
// 	Close();
// }

void UDPSocket::CreateUDPSocket( char const* portNum, const char* hostName /*= "" */ )
{
	UNUSED(hostName);
	addrinfo addrHints;
	ZeroMemory( &addrHints, sizeof( addrHints ) );

	memset( &m_toAddr, 0, sizeof(m_toAddr) );
	m_toAddr.sin_family = AF_INET;
	u_short iPortNum			= (u_short)atoi(portNum);
	m_toAddr.sin_port			= htons(iPortNum);
	m_toAddr.sin_addr.s_addr	= inet_addr(hostName);

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

void UDPSocket::SetHeader( std::uint16_t id, std::uint16_t length, std::uint32_t seq )
{
	m_sendBuffer.header.messageID	= id;
	m_sendBuffer.header.messageLen	= length;
	m_sendBuffer.header.messageSeq	= seq;
}

void UDPSocket::SetHeader( DataHeader header )
{
	m_sendBuffer.header = header;
}

void UDPSocket::WriteData( char const* dataPtr, int dataLen )
{
	memcpy( &m_sendBuffer.data[0], dataPtr, dataLen );
	m_sendBuffer.data[dataLen] = NULL;
}

std::string UDPSocket::ReadDataAsString()
{
	if( m_receiveBuffer.header.messageLen == 0 ) { return ""; }

	std::string result = std::string( m_receiveBuffer.data, m_receiveBuffer.header.messageLen );
	return result;
}

int UDPSocket::UDPSend( int length )
{
	int result = sendto( m_socket, (const char*)&m_sendBuffer, length, 0, reinterpret_cast<sockaddr*>(&m_toAddr), sizeof(m_toAddr) );
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
	int result = recvfrom( m_socket, (char*)&m_receiveBuffer, NET_BUFFER_SIZE, 0, reinterpret_cast<sockaddr*>(&fromAddr), &fromLen );
	if( result == SOCKET_ERROR ) {
		LOG_ERROR("Socket recv error: %d", WSAGetLastError() );
	}
	m_fromAddrText = std::string( inet_ntoa(fromAddr.sin_addr) ) + std::string( "  :") + std::to_string( htons(fromAddr.sin_port) );
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

