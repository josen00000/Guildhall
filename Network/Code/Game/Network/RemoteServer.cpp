#include "RemoteServer.hpp"
#include "Game/Entity.hpp"
#include "Game/Network/PlayerClient.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"

extern NetworkSystem* g_theNetworkSystem;

RemoteServer::RemoteServer( Game* game )
	:Server::Server(game)
{

}

void RemoteServer::Startup()
{
	__super::Startup();

	if( !m_isConnectFinished ) {
		SendTCPConnectionRequest();
		return;
	}
}

void RemoteServer::Shutdown()
{
	__super::Shutdown();
}

void RemoteServer::Update( float deltaSeconds )
{
	for( int i = 0; i < m_clients.size(); i++ ) {
		m_clients[i]->Update( deltaSeconds );
	}
	//m_playerClient->Update( deltaSeconds );
	m_game->Update( deltaSeconds );
// 	for( int i = 0; i < m_entities.size(); i++ ) {
// 		m_entities[i]->Update( deltaSeconds );
//	}
	Map* currentMap = m_game->m_world->GetCurrentMap();
	currentMap->CheckCollision();
}

void RemoteServer::BeginFrame()
{
	__super::BeginFrame();
}

void RemoteServer::EndFrame()
{
	__super::EndFrame();
}


void RemoteServer::ReceiveAndHandleTCPNetworkData()
{
	std::string tcpMessage;
	TCPClient* tempClient = g_theNetworkSystem->GetTCPClient();
	if( !tempClient->m_isRecvBufDirty ) {
		return;
	}
	tcpMessage = tempClient->GetStringFromData();
	tempClient->m_isRecvBufDirty = false;
	Strings result = SplitStringOnDelimiter( tcpMessage, "|" );
	if( result.size() == 2 ) {
		m_key = result[0];
		m_targetPort = result[1];
		tempClient->DisconnectServer();

		SendUDPConnectionRequest();
	}
	else {
		g_theConsole->DebugErrorf( "receive wrong TCP message: %s", tcpMessage.c_str() );
	}
}

void RemoteServer::ReceiveAndHandleUDPNetworkData()
{
	std::vector<GameInfo> gameinfos = g_theNetworkSystem->GetAllReceivedUDPMsgs();

	for( int i = 0; i < gameinfos.size(); i++ ) {
		if( gameinfos[i].first.size() == 0 ){ continue; }
		ParseAndExecuteRemoteMsg( gameinfos[i] );
	}
}

void RemoteServer::ParseAndExecuteRemoteMsg( GameInfo msg )
{
	std::string instruction = msg.second;
	Strings instructions = SplitStringOnDelimiter( instruction, "|" );
	
	if( instructions.size() == 4 ) {
		if( instructions[0] == "ENTITY" ) {
			if( instructions[1] == "CREATED" ) {
				Vec2 startPos;
				startPos.SetFromText( instructions[3].c_str() );
				m_entities.clear();
				// 			Entity* playerEntity = CreateAndPushEntityWithPos( "Marine", startPos );
				// 			m_playerClient->SetEntity( playerEntity );
				// 			
				m_playerIndex = std::stoi( instructions[2] );
				m_playerClient->m_entity->SetIsPlayer( true );
				m_playerClient->m_entity->Set2DPos( startPos );
				m_playerClient->m_gameCamera->SetPosition( m_playerClient->m_entity->GetPosition() );
				m_isConnectFinished = true;
			}
		}

	}
	else if( instructions.size() == 6 ) {
		if( instructions[0] == "ENTITY" ) {
			if( instructions[1] == "MOVE" ) {
				int entityIndex = stoi( instructions[2] );
				std::string entityName = instructions[3];
				Vec2 pos;
				pos.SetFromText( instructions[4].c_str() );
				float orient = std::stof( instructions[5] );

				if( m_entities.size() <= entityIndex ) {
					CreateAndPushEntityWithPos( entityName, pos );
				}
				else if( m_entities[entityIndex]->GetEntityName() != entityName ) {
					g_theConsole->DebugErrorf( "entity name is wrong" );
					//delete m_entities[entityIndex];
				}
				else {
					m_entities[entityIndex]->Set2DPos( pos );
					m_entities[entityIndex]->SetOrientation( orient );
					if( entityIndex == m_playerIndex ) {
						m_playerClient->m_gameCamera->SetPosition( m_entities[entityIndex]->GetPosition() );
					}
				}
			}
		}
	}
	else {
		g_theConsole->DebugErrorf( "remote server receive wrong msg" );
	}
}

void RemoteServer::SendNetworkData()
{
	if( m_isConnectFinished ) {
		if( m_playerClient->m_entity->GetIsMoving() ) {
			Vec2 moreDirt = m_playerClient->m_entity->GetMoveDirt();
			std::string gameMsg =	std::string( "ENTITY|MOVE|") + 
									moreDirt.ToString() + "|" +
									std::to_string( m_playerClient->m_entity->GetOrientation() );
			std::string targetIPAndPort = m_targetIPAddr + ":" + m_targetPort;
			GameInfo tempInfo = GameInfo( targetIPAndPort, gameMsg );
			g_theNetworkSystem->SetSendUDPData( tempInfo );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Mutator
//////////////////////////////////////////////////////////////////////////
void RemoteServer::SetIsConnectedFinished( bool isConnectFinished )
{	
	m_isConnectFinished = isConnectFinished;
}

void RemoteServer::SetTargetIPAddress( std::string targetAddr )
{
	m_targetIPAddr = targetAddr;
}

void RemoteServer::SendTCPConnectionRequest()
{
	g_theConsole->DebugLog( " in send tcp connection request " );
	TCPClient* client = g_theNetworkSystem->GetTCPClient();
	if( client == nullptr ) {
		ERROR_AND_DIE( "TCP client not create successful!" );
	}
	client->SendRequestConnectionMessageToServer();
}

void RemoteServer::SendUDPConnectionRequest()
{
	// create udpsocket
	// send request
	g_theConsole->DebugLogf( "send udp request :%s", m_targetPort.c_str() );
	UDPSocket* tempUDPSocket = g_theNetworkSystem->CreateUDPSocket();
	tempUDPSocket->CreateUDPSocket();
	tempUDPSocket->SetTargetPortAndIPAddress( m_targetPort.c_str(), m_targetIPAddr.c_str() );
	Port validPort = g_theNetworkSystem->GetValidPort();
	tempUDPSocket->BindSocket( atoi( validPort.c_str() )) ;
	std::string targetIPAndPort = std::string( m_targetIPAddr + " :" + m_targetPort );
	g_theNetworkSystem->SetSendUDPData( GameInfo( targetIPAndPort, m_key ));
	/*g_theNetworkSystem->SetSendUDPData( GameInfo)*/
}
