#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

#define WIN32_LEAN_AND_MEAN




#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "48000"

extern DevConsole* g_theConsole;

Server::Server()
{

}

void Server::StartUp()
{

}

void Server::ShutDown()
{
	if( m_clientSockets.size() > 0 ) {
		for( int i = 0; i < m_clientSockets.size(); i++ ) {
			//SendDataToClient( m_listenSockets[i], MESSAGE_ID::SERVER_DISCONNECT );
			DisconnectClientSocket( m_clientSockets[i] );
		}
	}
	if( m_listenSockets.size() > 0 ) {
		for( int i = 0; i < m_listenSockets.size(); i++ ) {
			CloseSocket( m_listenSockets[i] );
		}
	}
}

void Server::Update()
{
	if( m_isRecvBufDirty ) {
		g_theConsole->DebugLogf( "Handle data: %s", GetStringFromData().c_str() );
		m_isRecvBufDirty = false;
	}
}



void Server::BeginFrame()
{
	//memset( &m_sendBuf.data[0], '0', 520 );
	memset( &m_recvBuf.data[0], '0', NET_BUFFER_SIZE );
	m_isRecvBufDirty = false;
	if( m_listenSockets.size() > m_clientSockets.size() ) {
		FD_ZERO(&m_listenSet);
		for( int i = 0; i < m_listenSockets.size(); i++ ) {
			FD_SET( m_listenSockets[i], &m_listenSet );
		}
		int iResult = select( 0, &m_listenSet, NULL, NULL, &m_timeval );
		if( iResult == SOCKET_ERROR ) {
			g_theConsole->DebugErrorf( "Socket error in select: %d", WSAGetLastError() );
			return;
		}
		else if( iResult == 0 ) {
			//g_theConsole->DebugError( "Time limit expired in select" );
			//return;
		}

		// select success
		for( int i = 0; i < m_listenSockets.size(); i++ ) {
			if( FD_ISSET( m_listenSockets[i], &m_listenSet ) ) {
				AcceptConnectionFromClient( m_listenSockets[i] );
			}
		}
	}
	else if( m_listenSockets.size() < m_clientSockets.size() ) {
		g_theConsole->DebugErrorf( "Listen socket num not match!" );
	}
	else {
		for( int i = 0; i < m_clientSockets.size(); i++ ) {
			ReceiveDataFromClient( m_clientSockets[i] );

			if( m_isSendBufDirty ) {
				SendDataToClient( m_clientSockets[i], MESSAGE_ID::TEXT_MESSAGE );
			}
		}
		m_isSendBufDirty = false;
		memset( m_sendBuf.data, '0', m_sendBufLen );
		m_sendBufLen = 0;
	}
}

void Server::PrepareForClientConnectionWithPort( const char* portNum )
{
	addrinfo* socketAddr = NULL;
	addrinfo hints;
	ZeroMemory( &hints, sizeof( hints ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int iResult = getaddrinfo( NULL, portNum, &hints, &socketAddr );
	if( iResult != 0 ) {
		g_theConsole->DebugErrorf( "getaddrinfo failed with error: %d", iResult );
	}
	SOCKET tempSocket = CreateSocket( socketAddr );
	BindSocket( tempSocket, socketAddr );
	ListenOnSocket( tempSocket );
	m_listenSockets.push_back( tempSocket );
	g_theConsole->DebugLogf( "Ready for client connection in port: %s", portNum );
}

void Server::ReceiveDataFromClient( SOCKET clientSocket )
{
	DataPackage tempData;
	int iResult = recv( clientSocket, (char*)&tempData, NET_BUFFER_SIZE, 0 );
	if( iResult == SOCKET_ERROR ) {
		if( WSAGetLastError() != 10035 ) {
			DisconnectClientSocket( clientSocket );
			g_theConsole->DebugErrorf(" receive data error: %d", WSAGetLastError() );
		}
		return;
	}
	else if( iResult == 0 ) {
		g_theConsole->DebugError( "connection closed" );
		DisconnectClientSocket( clientSocket );
		return;
	}
	else {
		if( iResult < NET_BUFFER_SIZE ) {

			switch( tempData.header.messageID )
			{
			case TEXT_MESSAGE:
				WriteDataToRecvBuffer( tempData );
				break;
			case CLIENT_DISCONNECT:
				DisconnectClientSocket( clientSocket );
				g_theConsole->DebugLogf( "Client socket %d disconect",(int)clientSocket );
				break;
			default:
				g_theConsole->DebugError( "message ID error" );
				break;
			}
		}
	}
}

void Server::SendDataToClient( SOCKET clientSocket, MESSAGE_ID mid )
{
	DataPackage tempData;
	tempData.header.messageID = mid;

	switch( mid )
	{
	case SERVER_LISTENING:
		tempData.header.messageLen = (std::uint16_t)( sizeof(tempData.header) + m_sendBufLen ) ;
		memcpy( tempData.message, m_sendBuf.data, m_sendBufLen );
		break;
	case TEXT_MESSAGE:
		tempData.header.messageLen = (std::uint16_t)( sizeof(tempData.header) + m_sendBufLen );
		memcpy( tempData.message, m_sendBuf.data, m_sendBufLen );
		break;
	case SERVER_DISCONNECT:
		tempData.header.messageLen = (std::uint16_t)( sizeof(tempData.header) + 1 ) ;
		tempData.message[0] = '0';
		break;
	default:
		g_theConsole->DebugError( "Error in Message ID" );
		break;
	}

	int iResult = send( clientSocket, (char*)&tempData, m_sendBufLen + sizeof(tempData.header), 0 );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "Send data Error: %d", WSAGetLastError() );
		CloseSocket( clientSocket );
		return;
	}
	else {
		g_theConsole->DebugLogf( "Send data success with bytes: %d", iResult );
	}
}

void Server::SetSendData( const char* data, int dataLen )
{
	memcpy( m_sendBuf.data, data, dataLen );
	m_sendBuf.data[dataLen] = NULL;
	m_sendBufLen = dataLen;
	m_isSendBufDirty = true;
}

std::string Server::GetClientIPAddr( SOCKET clientSocket )
{
	sockaddr clientAddr;
	int addrSize = sizeof(clientAddr);
	int iResult = getpeername( clientSocket, &clientAddr, &addrSize );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "Get Client Address failed %d", WSAGetLastError() );
		return "";
	}
	char addrStr[128];
	DWORD outLen = (DWORD)128;
	iResult = WSAAddressToStringA( &clientAddr, addrSize, NULL, addrStr, &outLen );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "Change address to string error: %d", WSAGetLastError() );
		return "";
	}
	return std::string( addrStr );
}

std::string Server::GetStringFromData()
{
	return std::string( m_recvBuf.data );
}

SOCKET Server::CreateSocket( addrinfo* socketAddr )
{

	SOCKET tempSocket = socket( socketAddr->ai_family, socketAddr->ai_socktype, socketAddr->ai_protocol );
	if( tempSocket == INVALID_SOCKET ) {
		g_theConsole->DebugErrorf( "Create Socket error: %d", WSAGetLastError() );
		freeaddrinfo( socketAddr );
		//WSACleanup();
		return 0;
	}

	// ioControl for temp
	unsigned long blockingMode = 1;
	int iResult = ioctlsocket( tempSocket, FIONBIO, &blockingMode );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "Fail to IO Control socket: %d", WSAGetLastError() );
		return 0 ;
	}

	return tempSocket;
}

void Server::BindSocket( SOCKET socket, addrinfo* socketAddr )
{
	int iResult = bind( socket, socketAddr->ai_addr, (int)socketAddr->ai_addrlen );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "Bind socket error: %d", WSAGetLastError() );
		freeaddrinfo( socketAddr );
		closesocket( socket );
		return;
	}

	freeaddrinfo( socketAddr );
}

void Server::ListenOnSocket( SOCKET socket )
{
	int iResult = listen( socket, SOMAXCONN );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "listen error: %d", WSAGetLastError() );
		closesocket( socket );
		//WSACleanup();
	}
}

void Server::AcceptConnectionFromClient( SOCKET listenSocket )
{
	SOCKET clientSocket = accept( listenSocket, NULL, NULL );
	if( clientSocket == INVALID_SOCKET ) {
		g_theConsole->DebugErrorf( " accept failed %d", WSAGetLastError() );
		closesocket( listenSocket );
		return;
	}
	g_theConsole->DebugLogf( "client IP: %s", GetClientIPAddr( clientSocket).c_str() );
	const char* gameName = "game name is shit!";
	SetSendData( gameName, 20 );
	SendDataToClient( clientSocket, SERVER_LISTENING );
	memset( m_sendBuf.data, '0', m_sendBufLen );
	m_sendBufLen = 0;
	m_isSendBufDirty = false;
	m_clientSockets.push_back( clientSocket );
	g_theConsole->DebugLog( "Accept success." );
}

void Server::DisconnectClientSocket( SOCKET clientSocket )
{
	int iResult = shutdown( clientSocket, SD_SEND ); 
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "shundown error: %d", WSAGetLastError() );
	}

	CloseSocket( clientSocket );
}

void Server::CloseSocket( SOCKET socket )
{
	for( int i = 0; i < m_clientSockets.size(); i++ ) {
		if( socket == m_clientSockets[i] ) {
			m_clientSockets.erase( m_clientSockets.begin() + i );
		}
	}

	for( int i = 0; i < m_listenSockets.size(); i++ ) {
		if( socket == m_listenSockets[i] ) {
			m_listenSockets.erase( m_listenSockets.begin() + i );
		}
	}
	int iResult = closesocket( socket );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "close socket error: %d", WSAGetLastError() );
	}
}

void Server::WriteDataToRecvBuffer( DataPackage data )
{
	size_t bufferLen = data.header.messageLen - sizeof(data.header);
	memcpy( m_recvBuf.data, data.message, bufferLen );
	m_recvBuf.data[bufferLen] = NULL;
	g_theConsole->DebugLogf( "Receive data success from client with byte: %d", (int)bufferLen );
	m_isRecvBufDirty = true;
}
