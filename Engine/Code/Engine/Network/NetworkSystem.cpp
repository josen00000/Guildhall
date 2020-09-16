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
	m_server->BeginFrame();
}

void NetworkSystem::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	m_server->Update();
}

void NetworkSystem::EndFrame()
{

}

void NetworkSystem::Shutdown()
{

}

void NetworkSystem::StartServerWithPort( const char* port )
{
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
