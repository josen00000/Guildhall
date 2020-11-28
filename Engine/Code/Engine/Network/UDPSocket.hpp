#pragma once
#include <WinSock2.h>
#include <atomic>
#include <mutex>
//#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Network/NonBlockingQueue.hpp"
#include "Engine/Network/NonBlockingPriorityQueue.hpp"

struct ReliableUDPMsg {
	ReliableUDPMsg() = default;
	~ReliableUDPMsg() = default;
	ReliableUDPMsg( std::uint32_t iid, bool iIsAC, std::string iAddr, std::string iMsg ) 
		:id(iid)
		,isAC(iIsAC)
	{
		if( iAddr.size() > 50 || iMsg.size() > 50 ) {
			ERROR_RECOVERABLE( "msg size can not be larger than 50" );
		}
		addrLen = iAddr.size();
		msgLen = iMsg.size();
		memcpy( addr, iAddr.data(), iAddr.size() );
		memcpy( msg, iMsg.data(), iMsg.size() );
	}

	friend bool operator< ( ReliableUDPMsg a, ReliableUDPMsg b ) {
		return a.id > b.id;
	}

	std::uint32_t id = 0;
	bool isAC = false;
	uint addrLen = 0;
	uint msgLen = 0;
	char addr[50];
	char msg[50];
};

struct SocketBuffer {
	DataHeader header;
	char data[NET_MESSAGE_SIZE];
};

class UDPSocket {
public:
	UDPSocket() = default;
	~UDPSocket(){}

	void SetTargetPortAndIPAddress( char const* portNum, const char* hostName );
	void CreateUDPSocket();
	void BindSocket( int port ) ;

	// Mutator
	void SetHeader( std::uint32_t protocol, std::uint16_t id, std::uint16_t length, std::uint32_t seq );
	void SetHeader( DataHeader header );
	void AddReliableMsg( std::uint32_t seq, std::string addr, std::string msg );
	void WriteData( char const* dataPtr, int dataLen );
	void WriteReliableData( char const* dataPtr, int dataLen );
	void LockACData();
	void UnlockACData();
	void SetRecvReliableSequence( std::uint32_t sequence );

	// Accessor
	bool GetIsDataReliable();
	std::string ReadDataAsString();
	std::string GetRemoteIPAddr() const { return m_remoteIPAddr; }
	std::string GetRemoteIPAddrAndPort() const;
	std::string GetRemotePort() const { return m_remotePort; }
	std::string GetReceiveIPAddr() const { return m_fromAddrText; }

	std::uint32_t GetSendReliableSequence();
	std::uint32_t GetRecvReliableSequence() const { return m_recvReliableSequence; }

	DataHeader* GetReceiveHeader() { return &(m_receiveBuffer.header); }
	std::vector<ReliableUDPMsg> GetSendReliableMsg();

	int UDPSend( int length );
	int reliableUDPSend( int length ); 
	int UDPReceive();
	void Close();

public:
	std::atomic<bool>	m_isClose = false;
	std::uint32_t		m_sendReliableSequence = 0;
	std::uint32_t		m_recvReliableSequence = 0;
	SOCKET				m_socket = INVALID_SOCKET;
	addrinfo*			m_addrInfo = nullptr;
	SocketBuffer		m_sendBuffer;
	SocketBuffer		m_receiveBuffer;
	SocketBuffer		m_reliableReceiveBuffer;
	SocketBuffer		m_reliableSendBuffer;
	sockaddr_in			m_bindAddr;
	sockaddr_in			m_toAddr;
	NonBlockingPriorityQueue<ReliableUDPMsg> m_reliableRecvUDPMsgs{};
	NonBlockingPriorityQueue<ReliableUDPMsg> m_reliableSendUDPMsgs{};
	std::priority_queue<std::uint32_t, std::vector<std::uint32_t>, std::greater<std::uint32_t>> m_receivedACSequences;
	std::mutex			m_reliableDataMutex;
	std::string			m_fromAddrText;
	std::string			m_remoteIPAddr;
	std::string			m_remotePort;
	std::string			m_localPort;
};