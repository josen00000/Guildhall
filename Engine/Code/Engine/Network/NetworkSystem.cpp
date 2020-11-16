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

// 	m_TCPClient = new TCPClient();
// 	m_TCPServer = new TCPServer();
	//m_UDPSocket = new UDPSocket();
	
	m_readFromGameAndSendThread = std::thread( &NetworkSystem::ReadFromGameAndSendUDPMessage );
	m_receiveAndSendToGameThread = std::thread( &NetworkSystem::ReceiveUDPMessageAndWriteToGame );
}

void NetworkSystem::BeginFrame()
{
	if( m_TCPServer != nullptr ) {
		m_TCPServer->BeginFrame();
	}
	if( m_TCPClient != nullptr ) {
		m_TCPClient->BeginFrame();
	}
}

void NetworkSystem::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	if( m_TCPServer != nullptr ) {
		m_TCPServer->Update();
	}
	if( m_TCPClient != nullptr ) {
		m_TCPClient->Update();
	}
}

void NetworkSystem::EndFrame()
{

}

void NetworkSystem::Shutdown()
{
	if( m_TCPClient != nullptr ) {
		delete m_TCPClient;
	}
	if( m_TCPServer != nullptr ) {
		delete m_TCPServer;
	}
	WSACleanup();
	CloseUDPSockets();
	m_readFromGameAndSendThread.join();
	m_receiveAndSendToGameThread.join();
}

void NetworkSystem::StartTCPServerWithPort( const char* port )
{
	if( m_TCPServer == nullptr ) {
		m_TCPServer = new TCPServer();
	}
	m_TCPServer->PrepareForClientConnectionWithPort( port );
}

void NetworkSystem::CloseUDPSockets()
{
	for( int i = 0; i < m_UDPSockets.size(); i++ ) {
		m_UDPSockets[i]->Close();
	}
	//m_UDPSocket->Close();
	m_sendQueue.StopBlocking();
	m_isEnd = true;
}

UDPSocket* NetworkSystem::CreateUDPSocket()
{
	UDPSocket* tempUDPSocket = new UDPSocket();
	tempUDPSocket->CreateUDPSocket();
	m_UDPSockets.push_back( tempUDPSocket );
	return tempUDPSocket;
}

void NetworkSystem::SetUDPSocketTargetPortAndIPAddress( UDPSocket* socket, Port port, IPAddress addr )
{
	socket->SetTargetPortAndIPAddress( port.c_str(), addr.c_str() );
}

void NetworkSystem::SetUDPSocketBindPort( UDPSocket* socket, Port port )
{
	int iPort = atoi( port.c_str() );
	socket->BindSocket( iPort );
}

GameInfo NetworkSystem::GetReceivedUDPMsg()
{
	return m_receiveQueue.Pop();
}

std::vector<GameInfo> NetworkSystem::GetAllReceivedUDPMsgs()
{
	std::vector<GameInfo> result;
	result = m_receiveQueue.PopAll();
	return result;
}

std::string NetworkSystem::GetLocalIPAddress()
{
	char addrBuf[80];
	{
		int iResult = gethostname( addrBuf, sizeof(addrBuf) );
		if( iResult == SOCKET_ERROR ) {
			g_theConsole->DebugErrorf( " error in get local ip address", WSAGetLastError() );
		}
	}
	addrinfo hints;
	memset( &hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	//TODO
// 	struct addrinfo* info;
// 	{
// 		int iResult = getaddrinfo()
// 	}
	return std::string("test");
}

Port NetworkSystem::GetValidPort()
{
	static int portNum = 48000;
	portNum++;
	return std::to_string( portNum );
}

UDPSocket* NetworkSystem::FindUDPSocketWithLocalPort( Port port )
{
	for( int i = 0; i < m_UDPSockets.size(); i++ ) {
		if( m_UDPSockets[i]->m_localPort.compare( port ) == 0 ) {
			return m_UDPSockets[i];
		}
	}
	return nullptr;
}

UDPSocket* NetworkSystem::FindUDPSocketWithTargetIPAddressAndPort( IPAddress addr )
{
	addr = GetRidOfAllSpaceOfString( addr );
	for( int i = 0; i < m_UDPSockets.size(); i++ ) {
		std::string test = m_UDPSockets[i]->GetRemoteIPAddrAndPort();
		if( test == addr ) {
			return m_UDPSockets[i];
		}
	}
	g_theConsole->DebugErrorf( " Can not find socket with addr: %s", addr.c_str() );
	return nullptr;
}

void NetworkSystem::SetSendUDPData( GameInfo info )
{
	m_sendQueue.Push( info );
}

TCPClient* NetworkSystem::CreateTCPClient()
{
	TCPClient* tempClient = new TCPClient();
	//m_clients.push_back( tempClient );
	//m_TCPClient = tempClient;
	return tempClient;
}

void NetworkSystem::TCPConnectToServerWithIPAndPort( const char* hostName, const char* portNum )
{
	if( m_TCPClient != nullptr ) {
		m_TCPClient->ShutDown();
		delete m_TCPClient;
	}

	m_TCPClient = CreateTCPClient();
	m_TCPClient->ConnectToServerWithIPAndPort( hostName, portNum );
}

TCPServer* NetworkSystem::CreateTCPServer()
{
	TCPServer* tempServer = new TCPServer();
	//m_servers.push_back( tempServer );
	m_TCPServer = tempServer;
	return tempServer;
}

void NetworkSystem::ReadFromGameAndSendUDPMessage()
{
	static int index = 0;
	while( !g_theNetworkSystem->m_isEnd )
	{
		//g_theConsole->DebugLogf( "trying to send message with index : %d in thread: %d", index, std::this_thread::get_id() );
		GameInfo sendInfo = g_theNetworkSystem->m_sendQueue.Pop();
		UDPSocket* targetUDPSocket = g_theNetworkSystem->FindUDPSocketWithTargetIPAddressAndPort( sendInfo.first );
		if( !targetUDPSocket ){ continue; }
		std::string sendMsg = sendInfo.second;
		targetUDPSocket->SetHeader( UDP_HEADER_PROTOCOL, 0, (uint16_t)sendMsg.size(), index );
		targetUDPSocket->WriteData( sendMsg.data(), (int)sendMsg.size() );
		targetUDPSocket->UDPSend( (int)(sizeof(targetUDPSocket->m_sendBuffer.header) + sendMsg.size() + 1) );
		index++;	
	}
}

void NetworkSystem::ReceiveUDPMessageAndWriteToGame()
{
	DataHeader const* pMsg = nullptr;
	while( !g_theNetworkSystem->m_isEnd )
	{
		for( int i = 0; i < g_theNetworkSystem->m_UDPSockets.size(); i++ ){
			UDPSocket* socket = g_theNetworkSystem->m_UDPSockets[i];
			int size = socket->UDPReceive();
			if( size > 0 ) {
				//g_theConsole->DebugLogf( "receive message with in thread: %d", std::this_thread::get_id() );
				std::string dataStr;
				std::string IPAddr;
				IPAddr = /*std::string( "ip address is :") +*/ socket->GetReceiveIPAddr();
				dataStr = /*std::string( "received data is :") +*/ socket->ReadDataAsString();
				GameInfo result = GameInfo( IPAddr, dataStr );
				pMsg = socket->GetReceiveHeader();
				g_theNetworkSystem->m_receiveQueue.Push( result );
			}
		}
	}

}
