#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "UDPSocket.hpp"
#include "Engine/Network/NetworkSystem.hpp"

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

void UDPSocket::SetTargetPortAndIPAddress( char const* portNum, const char* hostName )
{
	//UNUSED(hostName);
	addrinfo addrHints;
	ZeroMemory( &addrHints, sizeof( addrHints ) );

	memset( &m_toAddr, 0, sizeof(m_toAddr) );
	m_toAddr.sin_family = AF_INET;
	u_short iPortNum			= (u_short)atoi(portNum);
	m_toAddr.sin_port			= htons(iPortNum);
	m_toAddr.sin_addr.s_addr	= inet_addr(hostName);
	m_remotePort = portNum;
	m_remoteIPAddr = hostName;
}

void UDPSocket::CreateUDPSocket()
{
	m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( m_socket == INVALID_SOCKET ) {
		LOG_ERROR( "Invalid socket" );
	}
}

void UDPSocket::BindSocket( int port )
{
	m_localPort = std::to_string( port );

	m_bindAddr.sin_family		= AF_INET;
	m_bindAddr.sin_port			= htons((u_short)port);
	m_bindAddr.sin_addr.s_addr	= INADDR_ANY;

	int result = bind( m_socket, reinterpret_cast<sockaddr*>(&m_bindAddr), sizeof(m_bindAddr) );
	if( result == SOCKET_ERROR ) {
		LOG_ERROR( "socket bind failed, error: %d", WSAGetLastError() );
	}
}

void UDPSocket::SetHeader( std::uint32_t protocol, std::uint16_t id, std::uint16_t length, std::uint32_t seq )
{
	m_sendBuffer.header.protocol	= protocol;
	m_sendBuffer.header.messageID	= id;
	m_sendBuffer.header.messageLen	= length;
	m_sendBuffer.header.messageSeq	= seq;
}

void UDPSocket::SetHeader( DataHeader header )
{
	m_sendBuffer.header = header;
}

void UDPSocket::SetReliableHeader( std::uint32_t protocol, std::uint16_t id, std::uint16_t length, std::uint32_t seq )
{
	m_reliableSendBuffer.header.protocol	= protocol;
	m_reliableSendBuffer.header.messageID	= id;
	m_reliableSendBuffer.header.messageLen	= length;
	m_reliableSendBuffer.header.messageSeq	= seq;
}

void UDPSocket::SetReliableHeader( DataHeader header )
{
	m_reliableSendBuffer.header = header;
}

void UDPSocket::AddReliableMsg( std::uint32_t seq, std::string addr, std::string msg )
{
	ReliableUDPMsg tempMsg( seq, true, addr, msg );
	if( addr.compare( "" ) == 0 || msg.compare( "" ) == 0 ) {
		int a = 0;
	}
	m_reliableSendUDPMsgs.push( tempMsg );
// 	if( m_reliableSendUDPMsgs.size() > 3 ) {
// 		ReliableUDPMsg testing = m_reliableSendUDPMsgs.top();
// 		int a  =0;
// 	}
}

void UDPSocket::WriteData( char const* dataPtr, int dataLen )
{
	memcpy( &m_sendBuffer.data[0], dataPtr, dataLen );
	m_sendBuffer.data[dataLen] = NULL;
}

void UDPSocket::WriteReliableData( char const* dataPtr, int dataLen )
{
	memcpy( &m_reliableSendBuffer.data[0], dataPtr, dataLen );
	m_reliableSendBuffer.data[dataLen] = NULL;
}

void UDPSocket::LockACData()
{
	m_reliableDataMutex.lock();
}

void UDPSocket::UnlockACData()
{
	m_reliableDataMutex.unlock();
}

void UDPSocket::SetRecvReliableSequence( std::uint32_t sequence )
{
	m_recvReliableSequence = sequence;
}

bool UDPSocket::GetIsDataReliable()
{
	if( m_receiveBuffer.header.messageID == RELIABLE_MSG ) {
		return true;
	}
	else if( m_receiveBuffer.header.messageID == NON_RELIABLE_MSG ) {
		return false;
	}
	else {
		return false;
	}
}

std::uint32_t UDPSocket::GetSendReliableSequence()
{
	std::uint32_t result = m_sendReliableSequence;
	m_sendReliableSequence++;
	return result;
}

std::string UDPSocket::ReadDataAsString()
{
	if( m_receiveBuffer.header.messageLen == 0 ) { return ""; }

	std::string result = std::string( m_receiveBuffer.data, m_receiveBuffer.header.messageLen );
	return result;
}

std::string UDPSocket::GetRemoteIPAddrAndPort() const
{
	return std::string( m_remoteIPAddr + ":" + m_remotePort );
}

std::vector<ReliableUDPMsg> UDPSocket::GetSendReliableMsg()
{
	// stupid way to do that
	std::vector<ReliableUDPMsg> result;
	while( !m_reliableSendUDPMsgs.empty() )
	{
		ReliableUDPMsg tempMsg = m_reliableSendUDPMsgs.top();
		result.push_back( tempMsg );
		m_reliableSendUDPMsgs.pop();
	}
	for( int i = 0; i < result.size(); i++ ) {
		m_reliableSendUDPMsgs.push( result[i] );
	}
	return result;
}

int UDPSocket::UDPSend( int length )
{
// 	std::string sendMsg = std::string( (const char*)&m_sendBuffer.data, length );
// 	g_theConsole->DebugLogf( "sending udp msg: %s",sendMsg.c_str() );
	int result = sendto( m_socket, (const char*)&m_sendBuffer, length, 0, reinterpret_cast<sockaddr*>(&m_toAddr), sizeof(m_toAddr) );
	if( result == SOCKET_ERROR ) {
		LOG_ERROR("Socket send error: %d", WSAGetLastError() );
	}
	return result;
}

int UDPSocket::reliableUDPSend( int length )
{
	std::string sendMsg = std::string( (const char*)&m_reliableSendBuffer.data, length );
	g_theConsole->DebugLogf( "sending reliable udp msg: %s",sendMsg.c_str() );
	int result = sendto( m_socket, (const char*)&m_reliableSendBuffer, length, 0, reinterpret_cast<sockaddr*>(&m_toAddr), sizeof( m_toAddr ) );
	if( result == SOCKET_ERROR ) {
		LOG_ERROR( "Socket send error: %d", WSAGetLastError() );
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
	else {
 		m_fromAddrText = std::string( inet_ntoa(fromAddr.sin_addr) ) + std::string( "  :") + std::to_string( htons(fromAddr.sin_port) );
		// Debug Info;
// 	 	std::string recvMsg = std::string( m_receiveBuffer.data, result );
// 	  	g_theConsole->DebugLogf( "receiving udp msg: %s", recvMsg.c_str() );

		// handle reliable udp message
		if( m_receiveBuffer.header.messageID == RELIABLE_MSG ) {
			IPAddress addr = GetReceiveIPAddr();
			std::string data = ReadDataAsString();
			std::uint32_t seq = m_receiveBuffer.header.messageSeq;
			if( data == "Received" ) {
				// AC package
				LockACData();
				m_receivedACSequences.push( seq );
				UnlockACData();
		 	  	g_theConsole->DebugLogf( "receiving ac package with seq: %i", seq );
			}
			else {
				// Reliable Msg
		 	  	g_theConsole->DebugLogf( "receiving reliable udp msg with seq: %i", seq );
				m_reliableRecvUDPMsgs.push( ReliableUDPMsg ( seq, true, addr, data ) );

				// send ac package
				// Does not know if ac package work
				std::string ACMsg = "Received";
				SocketBuffer ACPackage;
				ACPackage.header.messageID	= RELIABLE_MSG;
				ACPackage.header.messageSeq = seq;
				ACPackage.header.messageLen	= (int)ACMsg.size();
				memcpy( ACPackage.data, ACMsg.data(), ACMsg.size() );
				int result1 = sendto( m_socket, (const char*)&ACPackage, sizeof(ACPackage.header) + ACPackage.header.messageLen, 0, reinterpret_cast<sockaddr*>(&fromAddr), sizeof( fromAddr ) );
				if( result1 == SOCKET_ERROR ) {
					LOG_ERROR( "Socket send error: %d", WSAGetLastError() );
				}

			}
			return 0;

		}
	}
	return result;
}

void UDPSocket::Close()
{
	m_isClose = true;
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

