#include "RemoteClient.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Game/Entity.hpp"

extern NetworkSystem* g_theNetworkSystem;

RemoteClient::~RemoteClient()
{

}

RemoteClient::RemoteClient( Server* server )
	:Client::Client(server)
{

}

void RemoteClient::Startup()
{

}

void RemoteClient::Shutdown()
{

}

void RemoteClient::BeginFrame()
{
	if( m_entity != nullptr ) {
		m_entity->SetIsMoving( false );
	}
}

void RemoteClient::Update( float deltaSeconds )
{

}

void RemoteClient::EndFrame()
{

}

void RemoteClient::Render()
{

}

void RemoteClient::HandleInput( std::string input )
{
	Strings instructions = SplitStringOnDelimiter( input, "|" );
	Vec2 moveDirt;
	moveDirt.SetFromText( instructions[0].c_str() );

	float entityOrientation = std::stof( instructions[1]	);

	
	m_entity->SetMoveDirt( moveDirt );
	m_entity->SetIsMoving( true );
	m_entity->SetOrientation( entityOrientation );
}

// void RemoteClient::SendDataToRemoteServer()
// {
// 	std::string msg = std::string( m_clientInfo.dataBuf, m_clientInfo.dataLen );
// 	GameInfo tempInfo = GameInfo( m_clientInfo.IPAdress, msg );
// 	g_theNetworkSystem->SetSendUDPData( tempInfo );
// }

void RemoteClient::SendDataToRemoteServer( Strings data )
{
	Client::SendDataToRemoteServer( data );
}
