#include "TCPClient.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Network/NetworkSystem.hpp"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


extern DevConsole*	g_theConsole;



TCPClient::TCPClient()
{

}

TCPClient::~TCPClient()
{

}

void TCPClient::StartUp()
{
	m_isConnected = false;
}

void TCPClient::ShutDown()
{
	m_isConnected = false;
}

void TCPClient::BeginFrame()
{
	if( m_isConnected ) {
		FD_ZERO(&m_listenSet);
		FD_SET( m_targetSocket, &m_listenSet );

		int iResult = select( 0, &m_listenSet, NULL, NULL, &m_timeval );
		if( iResult == SOCKET_ERROR ) {
			g_theConsole->DebugErrorf( "Socket error in select: %d", WSAGetLastError() );
			return;
		}
		else if( iResult == 0 ) {
			//g_theConsole->DebugError( "Time limit expired in select" );
			//return;
		}

		if( m_isSendBufDirty ) {
			SendDataToServer( MESSAGE_ID::TEXT_MESSAGE );
		}
		ReceiveDataFromServer();
	}
}

void TCPClient::Update()
{
	if( m_isRecvBufDirty ) {
		g_theConsole->DebugLogf( "Handle data: %s", GetStringFromData().c_str() );
		m_isRecvBufDirty = false;
	}
}

std::string TCPClient::GetStringFromData()
{
	return std::string( m_recvBuf.data );
}

void TCPClient::SetTargetIP( const std::string& targetIP )
{
	m_targetIPAddress = targetIP;
}

void TCPClient::SetTargetPort( const std::string& targetPort )
{
	m_targetport = targetPort;
}

void TCPClient::SetSelfIP( const std::string& selfIP )
{
	m_IPAddress = selfIP;
}

void TCPClient::SetSelfPort( const std::string& selfPort )
{
	m_port = selfPort;
}

void TCPClient::ConnectToServerWithIPAndPort( const char* hostName, const char* portNum )
{
	CreateSocket( hostName, portNum );
	Connect();
}

void TCPClient::DisconnectServer()
{
	if( m_isConnected ) {
		SendDisconnectMessageToServer();
	}
	DisconnectAndCloseSocket();
}

void TCPClient::SetSendData( const char* data, int dataLen )
{
	memcpy( m_sendBuf.data, data, dataLen );
	m_sendBuf.data[dataLen] = NULL;
	m_sendBufLen = dataLen;
	m_isSendBufDirty = true;
}

void TCPClient::CreateSocket( const char* hostName, const char* portNum )
{
	addrinfo  addrHints;
	m_resultAddr = NULL;

	ZeroMemory( &addrHints, sizeof( addrHints ) );
	addrHints.ai_family = AF_INET;
	addrHints.ai_socktype = SOCK_STREAM;
	addrHints.ai_protocol = IPPROTO_TCP;


	int iResult = getaddrinfo( hostName, portNum, &addrHints, &m_resultAddr );
	if( iResult != 0 ) {
		g_theConsole->DebugErrorf( "Get addrinfo failed: %d \n", iResult );
		WSACleanup();
		return;
	}

	// ioControl
	m_targetSocket = socket( m_resultAddr->ai_family, m_resultAddr->ai_socktype, m_resultAddr->ai_protocol );
	if( m_targetSocket == INVALID_SOCKET ) {
		g_theConsole->DebugErrorf( "Error at socket(): %ld\n ", WSAGetLastError() );
		freeaddrinfo( m_resultAddr );
		WSACleanup();
		m_resultAddr = NULL;
		return;
	}
}

void TCPClient::CreateSocket( )
{
	CreateSocket( m_targetIPAddress.c_str(), m_targetport.c_str() );
}

void TCPClient::Connect()
{
	int iResult = connect( m_targetSocket, m_resultAddr->ai_addr, (int)m_resultAddr->ai_addrlen );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "connect error %d", WSAGetLastError() );
		closesocket( m_targetSocket );
		m_targetSocket = INVALID_SOCKET;
	}

	freeaddrinfo(m_resultAddr);

	if( m_targetSocket == INVALID_SOCKET ) {
		g_theConsole->DebugErrorf( "Server connection failed." );
		m_isConnected = false;
		return;
	}

	// connect successful
	m_isConnected = true;
	g_theConsole->DebugLog( "connect successful" );

	unsigned long blockingMode = 1;
	iResult = ioctlsocket( m_targetSocket, FIONBIO, &blockingMode );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "fail to IO Control socket: %d", WSAGetLastError() );
		return;
	}
}

void TCPClient::SendDataToServer( MESSAGE_ID mid )
{
	if( !m_isConnected ) { return; }

	DataPackage tempData;
	tempData.header.messageID = mid;

	switch( mid )
	{
	case TEXT_MESSAGE:
		tempData.header.messageLen = (std::uint16_t)(sizeof( tempData.header ) + m_sendBufLen );
		memcpy( tempData.message, m_sendBuf.data, m_sendBufLen );
		break;
	case CLIENT_DISCONNECT:
		tempData.header.messageLen = (std::uint16_t)(sizeof( tempData.header ) + 1);
		tempData.message[0] = '0';
		break;
	default:
		g_theConsole->DebugError( "Error in Message ID" );
		break;
	}

	int iResult = send( m_targetSocket, (char*)&tempData, m_sendBufLen + sizeof( tempData.header ), 0 );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "Send data Error: %d", WSAGetLastError() );
		closesocket(m_targetSocket);
		return;
	}
	else {
		g_theConsole->DebugLogf( "Send data success with bytes: %d", iResult );
		memset( m_sendBuf.data, '0', m_sendBufLen );
		m_sendBufLen = 0;
		m_isSendBufDirty = false;
	}
}

void TCPClient::SendDisconnectMessageToServer()
{
	if( m_isConnected ) {
		SendDataToServer( MESSAGE_ID::CLIENT_DISCONNECT );
	}
}

void TCPClient::ReceiveDataFromServer()
{
	DataPackage tempData = DataPackage();
	int iResult = recv( m_targetSocket, (char*)&tempData, NET_BUFFER_SIZE, 0 );
	if( iResult == SOCKET_ERROR ) {
		if( WSAGetLastError() != 10035 ) {
			g_theConsole->DebugErrorf( " receive data error: %d", WSAGetLastError() );
			DisconnectAndCloseSocket();
		}
		return;
	}
	else if( iResult == 0 ) {
		g_theConsole->DebugError( "connection closed" );
		DisconnectAndCloseSocket();
		return;
	}
	else {
		if( iResult < NET_BUFFER_SIZE ) {
			g_theConsole->DebugLogf( "Receive data %d byte success.", iResult );
			switch( tempData.header.messageID )
			{
			case SERVER_LISTENING:
				g_theConsole->DebugLogf( "server is listening!" );
			case TEXT_MESSAGE:
				WriteDataToRecvBuffer( tempData );
				break;
			case SERVER_DISCONNECT:
				DisconnectAndCloseSocket();
				g_theConsole->DebugLog( "server shut down" );
				break;
			default:
				g_theConsole->DebugError( "message ID error" );
				break;
			}
		}
	}
}


void TCPClient::ReceiveData()
{
	char recvBuf[512];
	int iResult = recv( m_targetSocket, recvBuf, 512, 0 );// temp buffer and temp buffer length
	if( iResult > 0 ) {
		g_theConsole->DebugLogf( "Byte received %d", iResult );
	}
	else if( iResult == 0 ) {
		g_theConsole->DebugLogf( "Connection closed in receive Data." );
		DisconnectAndCloseSocket();
	}
	else {
		g_theConsole->DebugErrorf( "Recv failed: %d", WSAGetLastError() );
	}
}

void TCPClient::DisconnectAndCloseSocket()
{
	int iResult = shutdown( m_targetSocket, SD_SEND );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "shut down failed: %d", WSAGetLastError() );
	}

	closesocket( m_targetSocket );
	m_targetSocket = INVALID_SOCKET;
	m_isConnected = false;
}

void TCPClient::WriteDataToRecvBuffer( DataPackage data )
{
	size_t bufferLen = data.header.messageLen - sizeof( data.header );
	memcpy( m_recvBuf.data, data.message, bufferLen );
	m_recvBuf.data[bufferLen] = NULL;
	g_theConsole->DebugLogf( "Receive data success from client with byte: %d", (int)bufferLen );
	m_isRecvBufDirty = true;
}
