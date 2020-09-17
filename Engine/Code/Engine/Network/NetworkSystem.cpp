#include "NetworkSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Network/Client.hpp"
#include "Engine/Network/Server.hpp"

#include <iostream>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")


extern DevConsole* g_theConsole;


NetworkSystem::NetworkSystem()
{
	
}

NetworkSystem::~NetworkSystem()
{

}

//COMMAND(startTCPServer, "Start TCP server listening" );

void NetworkSystem::StartUp()
{
	// Initialize winsock
	WSADATA wasData; // winsock implementation data
	WORD wVersion = MAKEWORD( 2, 2 );
	int iResult = WSAStartup( wVersion, &wasData );
	if( iResult != 0 ) {
		g_theConsole->DebugErrorf( "initial error: %d\n", iResult );
		return;
	}

	m_client = new Client();
	m_server = new Server();
}

void NetworkSystem::BeginFrame()
{
	if( m_server != nullptr ) {
		m_server->BeginFrame();
	}
	if( m_client != nullptr ) {
		m_client->BeginFrame();
	}
}

void NetworkSystem::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	if( m_server != nullptr ) {
		m_server->Update();
	}
	if( m_client != nullptr ) {
		m_client->Update();
	}
}

void NetworkSystem::EndFrame()
{

}

void NetworkSystem::Shutdown()
{
	if( m_client != nullptr ) {
		delete m_client;
	}
	if( m_server != nullptr ) {
		delete m_server;
	}
	WSACleanup();

}

void NetworkSystem::StartServerWithPort( const char* port )
{
	if( m_server == nullptr ) {
		m_server = new Server();
	}
	m_server->PrepareForClientConnectionWithPort( port );
}

Client* NetworkSystem::CreateClient()
{
	Client* tempClient = new Client();
	m_clients.push_back( tempClient );
	return tempClient;
}

Server* NetworkSystem::CreateServer()
{
	Server* tempServer = new Server();
	m_servers.push_back( tempServer );
	return tempServer;
}
