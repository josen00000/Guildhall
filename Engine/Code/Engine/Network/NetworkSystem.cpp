#include "NetworkSystem.hpp"
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>

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
