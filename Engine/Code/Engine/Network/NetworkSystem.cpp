#include "NetworkSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Network/TCPServer.hpp"
#include "Engine/Network/UDPSocket.hpp"

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
extern NetworkSystem* g_theNetworkSystem;

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

	m_client = new TCPClient();
	m_server = new TCPServer();
	m_UDPSocket = new UDPSocket();
	
	m_readFromGameAndSendThread = std::thread( &NetworkSystem::ReadFromGameAndSendMessage );
	m_receiveAndSendToGameThread = std::thread( &NetworkSystem::ReceiveMessageAndWriteToGame );
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
	CloseUDPSocket();
	m_readFromGameAndSendThread.join();
	m_receiveAndSendToGameThread.join();
}

void NetworkSystem::StartServerWithPort( const char* port )
{
	if( m_server == nullptr ) {
		m_server = new TCPServer();
	}
	m_server->PrepareForClientConnectionWithPort( port );
}

void NetworkSystem::CloseUDPSocket()
{
	m_UDPSocket->Close();
	m_sendQueue.StopBlocking();
	m_isEnd = true;
}

std::string NetworkSystem::GetReceivedMsg()
{
	return m_receiveQueue.Pop();
}

void NetworkSystem::SetSendMsg( std::string msg )
{
	m_sendQueue.Push( msg );
}

TCPClient* NetworkSystem::CreateClient()
{
	TCPClient* tempClient = new TCPClient();
	m_clients.push_back( tempClient );
	return tempClient;
}

TCPServer* NetworkSystem::CreateServer()
{
	TCPServer* tempServer = new TCPServer();
	m_servers.push_back( tempServer );
	return tempServer;
}

void NetworkSystem::ReadFromGameAndSendMessage()
{
	static int index = 0;
	while( !g_theNetworkSystem->m_isEnd )
	{
		g_theConsole->DebugLogf( "trying to send message with index : %d in thread: %d", index, std::this_thread::get_id() );
		std::string sendMes = g_theNetworkSystem->m_sendQueue.Pop();
		g_theNetworkSystem->m_UDPSocket->SetHeader( 0, (uint16_t)sendMes.size(), index );
		g_theNetworkSystem->m_UDPSocket->WriteData( sendMes.data(), (int)sendMes.size() );
		g_theNetworkSystem->m_UDPSocket->UDPSend( (int)(sizeof(g_theNetworkSystem->m_UDPSocket->m_sendBuffer.header) + sendMes.size() + 1) );
		index++;	
	}
}

void NetworkSystem::ReceiveMessageAndWriteToGame()
{
	DataHeader const* pMsg = nullptr;
	UDPSocket* socket = g_theNetworkSystem->m_UDPSocket;
	while( !g_theNetworkSystem->m_isEnd )
	{
		int size = g_theNetworkSystem->m_UDPSocket->UDPReceive();
		if( size > 0 ) {
			g_theConsole->DebugLogf( "receive message with in thread: %d", std::this_thread::get_id() );
			std::string dataStr;
			dataStr = std::string( "ip address is :") + socket->GetReceiveIPAddr();
			dataStr += std::string( "received data is :") + socket->ReadDataAsString();
			pMsg = socket->GetReceiveHeader();
			g_theNetworkSystem->m_receiveQueue.Push( dataStr );
		}
	}

}
