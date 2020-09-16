#include "Client.hpp"
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



Client::Client()
{

}

void Client::StartUp()
{
	m_isConnected = false;
}

void Client::ShutDown()
{
	m_isConnected = false;
}

void Client::SetTargetIP( const std::string& targetIP )
{
	m_targetIPAddress = targetIP;
}

void Client::SetTargetPort( const std::string& targetPort )
{
	m_targetport = targetPort;
}

void Client::SetSelfIP( const std::string& selfIP )
{
	m_IPAddress = selfIP;
}

void Client::SetSelfPort( const std::string& selfPort )
{
	m_port = selfPort;
}

void Client::SendMessage( std::string message )
{
	m_message = message;
	SendData();
}

void Client::CreateSocket( const char* hostName, const char* portNum )
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

	m_socket = socket( m_resultAddr->ai_family, m_resultAddr->ai_socktype, m_resultAddr->ai_protocol );
	if( m_socket == INVALID_SOCKET ) {
		g_theConsole->DebugErrorf( "Error at socket(): %ld\n ", WSAGetLastError() );
		freeaddrinfo( m_resultAddr );
		WSACleanup();
		m_resultAddr = NULL;
		return;
	}
}

void Client::CreateSocket( )
{
	CreateSocket( m_targetIPAddress.c_str(), m_targetport.c_str() );
}

void Client::Connect()
{
	int iResult = connect( m_socket, m_resultAddr->ai_addr, (int)m_resultAddr->ai_addrlen );
	if( iResult == SOCKET_ERROR ) {
		closesocket( m_socket );
		m_socket = INVALID_SOCKET;
	}

	freeaddrinfo(m_resultAddr);

	if( m_socket == INVALID_SOCKET ) {
		g_theConsole->DebugErrorf( "Server connection failed." );
		WSACleanup();
		m_isConnected = false;
		return;
	}

	// connect successful
	m_isConnected = true;
}

void Client::SendData()
{
	if( !m_isConnected ) {
		g_theConsole->DebugError( "connection build failed. Can not send data. " );
	}
	const char* sendBuf = "testing!";


	int iResult = send( m_socket, m_message.data(), (int)m_message.size(), 0 );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "send failed %d\n", WSAGetLastError() );
		closesocket( m_socket );
		WSACleanup();
		return;
	}

	g_theConsole->DebugLogf( "bytes sent : %d\n", iResult );

	// shutdown?
// 	int iResult = shutdown( m_socket, SD_SEND );
// 	if( iResult == SOCKET_ERROR ) {
// 		g_theConsole->DebugErrorf(" shut down fails %d ", WSAGetLastError() );
// 		closesocket( m_socket );
// 		WSACleanup();
// 		return;
// 	}

}

void Client::ReceiveData()
{
	char recvBuf[512];
	int iResult = recv( m_socket, recvBuf, 512, 0 );// temp buffer and temp buffer length
	if( iResult > 0 ) {
		g_theConsole->DebugLogf( "Byte received %d", iResult );
	}
	else if( iResult == 0 ) {
		g_theConsole->DebugLogf( "Connection closed in receive Data." );
	}
	else {
		g_theConsole->DebugErrorf( "Recv failed: %d", WSAGetLastError() );
	}
}

void Client::DisConnect()
{
	int iResult = shutdown( m_socket, SD_SEND );
	if( iResult == SOCKET_ERROR ) {
		g_theConsole->DebugErrorf( "shut down failed: %d", WSAGetLastError() );
	}

	closesocket( m_socket );
	m_socket = INVALID_SOCKET;
	WSACleanup();
}
