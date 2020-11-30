#include "AuthoritativeServer.hpp"
#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/Network/RemoteClient.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Network/TCPServer.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

extern NetworkSystem*	g_theNetworkSystem;
extern InputSystem*		g_theInputSystem;

AuthoritativeServer::AuthoritativeServer( Game* game )
	:Server::Server(game)
{
	m_rng = new RandomNumberGenerator();
}

AuthoritativeServer::~AuthoritativeServer() {
	delete m_rng;
}

void AuthoritativeServer::Startup()
{
	__super::Startup();
	
}

void AuthoritativeServer::Shutdown()
{
	__super::Shutdown();
}

void AuthoritativeServer::Update( float deltaSeconds )
{
	__super::Update( deltaSeconds );
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_T ) ) {
		ReliableUDPMessageTest();
	}
}

void AuthoritativeServer::BeginFrame()
{
	// 1 check if listening tcp clients
	// 2 check if client pass the approvement
	// 3 send key and open UDP port for remote server
	// 4 save the integer and ip address
	
	// only handle one waiting client at one frame
	__super::BeginFrame();
}

void AuthoritativeServer::EndFrame()
{
	__super::EndFrame();
}

void AuthoritativeServer::ReceiveAndHandleTCPNetworkData()
{
	// handle tcp network data
	TCPServer* tempTCPServer = g_theNetworkSystem->GetTCPServer();
	if( tempTCPServer == nullptr ){ return; }
	if( tempTCPServer->IsWaitingClients() ) {
		SOCKET waitClientSocket = tempTCPServer->GetWaitingClientSocket();
		IPAddress addr = tempTCPServer->GetClientIPAddressWithSocket( waitClientSocket );
		if( !CheckIfClientApproved( addr ) ) {
			// TODO error handling;
			return;
		}
		PrepareConnectionForClientWithIPAddress( addr );
	}
}

void AuthoritativeServer::ReceiveAndHandleUDPNetworkData()
{
	std::vector<GameInfo> gameInfos = g_theNetworkSystem->GetAllReceivedUDPMsgs();


	for( int i = 0; i < gameInfos.size(); i++ ) {
		if( gameInfos[i].m_addr.size() == 0 ){ continue; }
		ParseAndExecuteRemoteMsg( gameInfos[i] );
	}

}

void AuthoritativeServer::SendGameMsg( )
{

}

void AuthoritativeServer::SendNetworkData()
{
	// Generate data for this frame
	// send data to all client
	
	Strings gameMsgs;
	for( int i = 0; i < m_entities.size(); i++ ) {
		Entity* tempEntity = m_entities[i];
		Vec2 currentPos		= tempEntity->Get2DPosition();
		float currentOrient	= tempEntity->GetOrientation();
		std::string gameMsg =	std::string( "ENTITY|MOVE|" ) +
								std::to_string( i ) + "|" +
								tempEntity->GetEntityName() + "|" +
								currentPos.ToString() + "|" + 
								std::to_string( currentOrient );

		std::string gameHealthMsg = std::string( "ENTITY|HEALTH|" ) +
									std::to_string( i ) + "|" +
									std::to_string( tempEntity->GetHP() );
		if( tempEntity->GetIsDead() ) {

			std::string gameDeadMsg =	std::string( "ENTITY|DEAD|" ) +
										std::to_string( i );
			gameMsgs.push_back( gameDeadMsg );
		}
		gameMsgs.push_back( gameMsg );
		gameMsgs.push_back( gameHealthMsg );
	}

	for( auto iter = m_clients.begin(); iter != m_clients.end(); ++iter ) {
			(*iter)->SendDataToRemoteServer( gameMsgs );
	}

}

Client* AuthoritativeServer::FindClientWithIPAddress( IPAddress addr )
{
	for( auto iter = m_clients.begin(); iter != m_clients.end(); ++iter ) {
		IPAddress tempAddr = (*iter)->GetIPAddress();
		Strings addrs = SplitStringOnDelimiter( tempAddr, ":" );
		if( addrs[0].compare( addr ) == 0 ) {
			return (*iter);
		}
	}
	g_theConsole->DebugErrorf( "client ip address not find" );
	return nullptr;
}

Client* AuthoritativeServer::FindClientWithIPAddressAndPort( IPAddress addr )
{
	for( auto iter = m_clients.begin(); iter != m_clients.end(); ++iter ) {
		IPAddress tempAddr = (*iter)->GetIPAddress();
		if( tempAddr.compare( addr ) == 0 ) {
			return (*iter);
		}
	}
	g_theConsole->DebugErrorf( "client ip address not find" );
	return nullptr;
}

void AuthoritativeServer::CreateAndPushPlayer()
{
	
}

void AuthoritativeServer::CreateAndPushRemoteClient( ClientInfo info )
{
	RemoteClient* tempClient = new RemoteClient( this );
	tempClient->SetClientInfo( info );
	m_clients.push_back( tempClient );
}

void AuthoritativeServer::CreateAndPushRemotePlayerEntityToClient( Client* client, Vec2 startPos )
{
	Entity* clientEntity = CreateAndPushEntityWithPos( "Marine", startPos );
	client->SetEntity( clientEntity );
}

void AuthoritativeServer::MoveClientEntity( Client* client, std::string instruction )
{
	client->HandleInput( instruction );
}

void AuthoritativeServer::PrepareConnectionForClientWithIPAddress( IPAddress addr )
{
	g_theConsole->DebugLogf("prepare connection for :%s", addr.c_str() );
	Port port = g_theNetworkSystem->GetValidPort();
	UDPSocket* tempUDPSocket = g_theNetworkSystem->CreateUDPSocket();
	g_theNetworkSystem->SetUDPSocketBindPort( tempUDPSocket, port );
	
	int key = m_rng->RollRandomIntLessThan( INT_MAX );


	ClientInfo tempClientInfo = ClientInfo();
	tempClientInfo.key			= std::to_string( key );
	tempClientInfo.isConnected	= false;
	tempClientInfo.IPAdress		= addr;
	tempClientInfo.localPort	= port;

	CreateAndPushRemoteClient( tempClientInfo );

	TCPServer* tempServer = g_theNetworkSystem->GetTCPServer();
	tempServer->SendReadyForConnectionMessageToClientWithIPAddress( addr, std::to_string( key ), port );
}

bool AuthoritativeServer::CheckIfClientApproved( IPAddress addr )
{
	// TODO add validate function
	UNUSED(addr);
	return true;
}

void AuthoritativeServer::ReliableUDPMessageTest()
{
	static int a = 0;
	for( int i = 0; i < 5; i++ ) {
		if( m_clients.size() > 0 ) {
			std::string testMsg = "testing" + std::to_string( a );
			for( auto iter = m_clients.begin(); iter != m_clients.end(); ++iter ) {
				(*iter)->SendReliableDataToRemoteServer( testMsg );
			}
			a++;
		}
	}
}

void AuthoritativeServer::ParseAndExecuteRemoteMsg( GameInfo msg )
{
	std::string clientAddr	= msg.m_addr;
	std::string gameMsg		= msg.m_msg;

	Strings addrs = SplitStringOnDelimiter( clientAddr, ":" );
	addrs[0] = GetStringWithoutSpace( addrs[0] );
	Client* tempClient = FindClientWithIPAddressAndPort( clientAddr );
	if( !tempClient ) {
		tempClient = FindClientWithIPAddress( addrs[0] );
		if( !tempClient ) {
			ERROR_RECOVERABLE( "no client find with ip:%s", addrs[0].c_str() );
		}
		else {
			if( gameMsg == tempClient->GetKey() ) {
				tempClient->SetIPAddress( clientAddr );
				tempClient->SetIsConnected( true );
				Vec2 startPos = Vec2( 3.f, 1.5f );
				CreateAndPushRemotePlayerEntityToClient( tempClient, startPos );

				UDPSocket* socket = g_theNetworkSystem->FindUDPSocketWithLocalPort( tempClient->GetLocalPort() );
				g_theNetworkSystem->SetUDPSocketTargetPortAndIPAddress( socket, addrs[1], addrs[0] );
				std::string msg =	std::string( "ENTITY|CREATED|") +
									std::to_string( m_entities.size()-1 ) + "|" + 
									startPos.ToString();
				tempClient->SendReliableDataToRemoteServer( msg );
			}
			else {
				g_theConsole->DebugErrorf( " key does not match: %s", gameMsg.c_str() );
			}
			return;
		}
	}

	tempClient->m_entity->SetIsMoving( false );
	Strings gameMsgs = SplitStringOnDelimiter( gameMsg, "|" , 2 );
	if( gameMsgs[0].compare( "ENTITY" ) == 0 ) {
		if( gameMsgs[1].compare( "MOVE" ) == 0 ) {
			MoveClientEntity( tempClient, gameMsgs[2] );
		}
		else if( gameMsgs[1].compare( "SHOOT" )  == 0 ) {
			int entityIndex = std::stoi( gameMsgs[2] );
			HandleShoot( entityIndex );
		}
	}
	else {
		g_theConsole->DebugErrorf( "invalid remote msg " );
	}

}

void AuthoritativeServer::HandleShoot( int entityIndex )
{
 	Map* currentMap = m_game->m_world->GetCurrentMap();
 	Entity* shootEntity = GetEntityWithIndex( entityIndex );
	Vec2 startPos = shootEntity->Get2DPosition();
	Vec2 shootDirt = Vec2::ONE_ZERO;
	shootDirt.SetAngleDegrees( shootEntity->GetOrientation() );
	startPos += ( shootDirt * 1.5f );
	Vec2 endPos = startPos + ( shootDirt * 5 );
	MapRaycastResult result = currentMap->RayCast( Vec3( startPos, 0.5f ), Vec3( endPos, 0.5f )) ;
	if( result.impactEntity ) {
		Entity* shotEntity = result.impactEntity;
		shotEntity->GetShot();
	}
}
