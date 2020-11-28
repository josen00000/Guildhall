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
	//m_receiveAndSendToGameThread = std::thread( &NetworkSystem::ReceiveUDPMessageAndWriteToGame );
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
	//m_receiveAndSendToGameThread.join();
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
		m_udpReceiveThreads[i].join();
		m_udpReliableSendThreads[i].join();
		m_udpReliableWriteToGameThreads[i].join();
	}
	m_udpReliableWriteToGameThreads.clear();
	m_udpReliableSendThreads.clear();
	m_udpReceiveThreads.clear();
	m_sendQueue.StopBlocking();
	m_isEnd = true;
}

UDPSocket* NetworkSystem::CreateUDPSocket()
{
	UDPSocket* tempUDPSocket = new UDPSocket();
	tempUDPSocket->CreateUDPSocket();
	std::thread tempRecvThread ( &NetworkSystem::ReceiveUDPMessageAndWriteToGame, tempUDPSocket );
	std::thread tempReliableSendThread ( &NetworkSystem::SendReliableUDPMessage, tempUDPSocket );
	std::thread tempReliableWriteGameThread ( &NetworkSystem::WriteReliableUDPMessageToGame, tempUDPSocket );

	m_UDPSockets.push_back( tempUDPSocket );
	m_udpReceiveThreads.push_back( std::move(tempRecvThread) );
	m_udpReliableSendThreads.push_back( std::move( tempReliableSendThread ) );
	m_udpReliableWriteToGameThreads.push_back( std::move( tempReliableWriteGameThread ) );
	
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

GameInfo NetworkSystem::GetGameInfoWithReliableUDPMessage( ReliableUDPMsg msg )
{
	GameInfo tempInfo;
	tempInfo.m_addr = std::string( msg.addr, msg.addrLen );
	tempInfo.m_isReliable = true;
	tempInfo.m_msg = std::string( msg.msg, msg.msgLen );
	return tempInfo;
}

std::vector<GameInfo> NetworkSystem::GetAllReceivedUDPMsgs()
{
	std::vector<GameInfo> result;
	if( !m_receiveQueue.Empty() ){
		result = m_receiveQueue.PopAll();
	}
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
		UDPSocket* targetUDPSocket = g_theNetworkSystem->FindUDPSocketWithTargetIPAddressAndPort( sendInfo.m_addr );
		if( !targetUDPSocket ){ 
			continue; 
		}
		std::string sendMsg = sendInfo.m_msg;
		if( sendInfo.m_isReliable ) {
			std::uint32_t sequence = targetUDPSocket->GetSendReliableSequence();
			targetUDPSocket->SetHeader( UDP_HEADER_PROTOCOL, RELIABLE_MSG, (uint16_t)sendMsg.size(), sequence );
			targetUDPSocket->AddReliableMsg( sequence, sendInfo.m_addr, sendMsg );
		}
		else {
			targetUDPSocket->SetHeader( UDP_HEADER_PROTOCOL, NON_RELIABLE_MSG, (uint16_t)sendMsg.size(), index );
			index++;	
		}
		targetUDPSocket->WriteData( sendMsg.data(), (int)sendMsg.size() );
		targetUDPSocket->UDPSend( (int)(sizeof(targetUDPSocket->m_sendBuffer.header) + sendMsg.size() + 1) );
	}
}

void NetworkSystem::WriteReliableUDPMessageToGame( UDPSocket* socket )
{
	while( !socket->m_isClose ) {
		if( socket->m_reliableRecvUDPMsgs.empty() ) {
			continue;
		}
		ReliableUDPMsg topMsg;
		while( !socket->m_reliableRecvUDPMsgs.empty() )
		{
			// Get rid of duplicate msg
			topMsg = socket->m_reliableRecvUDPMsgs.top();
			if( topMsg.id < socket->GetRecvReliableSequence() ) {
				socket->m_reliableRecvUDPMsgs.pop();
			}
			else {
				break;
			}
		}
		if( topMsg.id == socket->GetRecvReliableSequence() ) {
			GameInfo tempInfo = g_theNetworkSystem->GetGameInfoWithReliableUDPMessage( topMsg );
			if( &tempInfo == NULL ) {
				int a = 0;
			}
			g_theNetworkSystem->m_receiveQueue.Push( tempInfo );
			socket->SetRecvReliableSequence( ( topMsg.id + 1 ) );
		}

		std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
	}
}

void NetworkSystem::ReceiveUDPMessageAndWriteToGame( UDPSocket* socket )
{
	DataHeader const* pMsg = nullptr;
	while( !socket->m_isClose )
	{
		int size = socket->UDPReceive();
		if( size > 0 ) {
			//g_theConsole->DebugLogf( "receive message with in thread: %d", std::this_thread::get_id() );
			std::string dataStr;
			std::string IPAddr;
			IPAddr = /*std::string( "ip address is :") +*/ socket->GetReceiveIPAddr();
			dataStr = /*std::string( "received data is :") +*/ socket->ReadDataAsString();
			bool isReliable = socket->GetIsDataReliable();
			GameInfo result = GameInfo( isReliable, IPAddr, dataStr );
			if( &result == NULL ) {
				int a =0;
			}
			pMsg = socket->GetReceiveHeader();
			g_theNetworkSystem->m_receiveQueue.Push( result );
		}
		//std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
	}

}

void NetworkSystem::SendReliableUDPMessage( UDPSocket* socket )
{
	while( !socket->m_isClose ) {
		// Check ac package
		// Resend reliable message
		if( socket->m_reliableSendUDPMsgs.empty() ) {
			std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
			continue;
		}
		socket->LockACData();
		// get rid of AC package received message
		if( !socket->m_receivedACSequences.empty() ) {
			std::uint32_t ACseq; 
			ReliableUDPMsg topSendMsg; 
			while( !socket->m_receivedACSequences.empty() && !socket->m_reliableSendUDPMsgs.empty() ) {
				ACseq		= socket->m_receivedACSequences.top();
				topSendMsg	= socket->m_reliableSendUDPMsgs.top();
				if( ACseq < topSendMsg.id ) {
					socket->m_receivedACSequences.pop();
				}
				else if ( ACseq == topSendMsg.id ) {
					socket->m_receivedACSequences.pop();
					socket->m_reliableSendUDPMsgs.pop();
				}
				else {
					break;
				}
			}
		}

		// send un AC Msg
		if( !socket->m_reliableSendUDPMsgs.empty() ) {
			std::vector<ReliableUDPMsg> sendReliableMsgs = socket->GetSendReliableMsg();
			for( int i = 0; i < sendReliableMsgs.size(); i++ ) {
				ReliableUDPMsg tempMsg = sendReliableMsgs[i];
				std::string sendMsg = std::string( tempMsg.msg, tempMsg.msgLen );
				std::uint32_t sequence = tempMsg.id;
				
				socket->SetHeader( UDP_HEADER_PROTOCOL, RELIABLE_MSG, (uint16_t)sendMsg.size(), sequence );
				socket->WriteReliableData( sendMsg.data(), (int)sendMsg.size() );
				socket->reliableUDPSend( (int)(sizeof( socket->m_reliableSendBuffer.header ) + sendMsg.size() + 1) );
			}
		}
		socket->UnlockACData();
		std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
	}
}
