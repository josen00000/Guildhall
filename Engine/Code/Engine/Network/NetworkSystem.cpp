#include "NetworkSystem.hpp"
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>

NetworkSystem::NetworkSystem()
{
	
}

//COMMAND(startTCPServer, "Start TCP server listening" );

void NetworkSystem::StartUp()
{
	// Initialize winsock
	WSADATA wasData;
	WORD wVersion = MAKEWORD( 2, 2 );
	int iResult = WSAStartup( wVersion, &wasData );
	if( iResult != 0 ) {
		// log error
		return;
	}
}

void NetworkSystem::BeginFrame()
{
	if( m_isListening ) {
		struct addrinfo addrHintsIn;
		struct addrinfo* pAddrOut;

		ZeroMemory( &addrHintsIn, sizeof(addrHintsIn) );
		addrHintsIn.ai_family = AF_INET;
		addrHintsIn.ai_socktype = SOCK_STREAM;
		addrHintsIn.ai_protocol = IPPROTO_TCP;
		addrHintsIn.ai_flags	= AI_PASSIVE;

		std::string serverPort("48000");
		int iResult = getaddrinfo( NULL,  serverPort.c_str(), &addrHintsIn, &pAddrOut );
		if( iResult != 0 ) {
			// debug error
		}

		// m_pListenSocket = socket( pAddrOut->ai_family, pAddrOut->ai_sock...);
	}
}

void NetworkSystem::Update()
{

}

void NetworkSystem::EndFrame()
{

}

void NetworkSystem::ShutDown()
{

}
