#include "Client.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/DevConsole.hpp"

extern NetworkSystem*	g_theNetworkSystem;
extern DevConsole*		g_theConsole;

Client::~Client()
{
	//SELF_SAFE_RELEASE(m_entity);
// 	delete m_entity;
// 	m_entity = nullptr;
}

Client::Client( Server* owner )
{
	m_owner = owner;
}

void Client::SetEntity( Entity* entity )
{
	m_entity = entity;
}

void Client::SetKey( std::string key )
{
	m_clientInfo.key = key;
}

void Client::SetIPAddress( std::string addr )
{
	m_clientInfo.IPAdress = addr;
}

void Client::SetIsConnected( bool isConnected )
{
	m_clientInfo.isConnected = isConnected;
}

void Client::SetIsDataReadyForSend( bool isDataReady )
{
	m_clientInfo.isDataReadyForSend = isDataReady;
}

void Client::SetSendMessage( std::string msg )
{
	memcpy( m_clientInfo.dataBuf, msg.data(), msg.size() );
	m_clientInfo.dataLen = msg.size();
}

void Client::SetClientInfo( ClientInfo info )
{
	m_clientInfo = info;
}

void Client::Startup()
{

}

void Client::Shutdown()
{

}

void Client::BeginFrame()
{

}

void Client::Update( float deltaSeconds )
{

}

void Client::EndFrame()
{

}

void Client::Render()
{

}


void Client::SendDataToRemoteServer( Strings data )
{
	std::string addr = m_clientInfo.IPAdress;
	if( GetIsConnected() ) {
		for( int i = 0; i < data.size(); i++ ) {
			GameInfo tempInfo = GameInfo( addr, data[i] );
			g_theNetworkSystem->SetSendUDPData( tempInfo );
		}
	}
	else {
		g_theConsole->DebugErrorf( "try send data to remote server not connected" );
	}
}

void Client::SendDataToRemoteServer( std::string data )
{
	std::string addr = m_clientInfo.IPAdress;
	if( GetIsConnected() ) {
		GameInfo tempInfo = GameInfo( addr, data );
		g_theNetworkSystem->SetSendUDPData( tempInfo );
	}
	else {
		g_theConsole->DebugErrorf( "try send data to remote server not connected" );
	}
}
