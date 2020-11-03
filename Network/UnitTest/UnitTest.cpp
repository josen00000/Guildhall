#include "pch.h"
#include "CppUnitTest.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h> 
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Network/UDPSocket.cpp"
#include <WS2tcpip.h>
#include <array>
#include <mutex>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

/*#define TEST_MODE*/
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		TEST_METHOD_INITIALIZE(InitUDPSocketTest){ 
			WSADATA wasData; // winsock implementation data
			WORD wVersion = MAKEWORD( 2, 2 );
			int iResult = WSAStartup( wVersion, &wasData );
			if( iResult != 0 ) {
				//g_theConsole->DebugErrorf( "initial error: %d\n", iResult );
				return;
			}
		}

		using TextArray = std::array<std::string, 5>;
		TextArray text ={
			"hello world1.",
			"hello world2.",
			"hello world3.",
			"hello world4.",
			"hello world5."
		};
		std::uint16_t toU16( std::size_t input ){ return static_cast<std::uint16_t>(input); }

		using MessageArray = std::array<std::tuple<std::uint16_t, std::uint16_t, std::uint16_t, std::string>, 5>;
		MessageArray messages = {
			std::make_tuple( 1, 0, toU16(strlen(text[0].c_str())), text[0] ),
			std::make_tuple( 1, 1, toU16(strlen(text[0].c_str())), text[1] ),
			std::make_tuple( 1, 2, toU16(strlen(text[0].c_str())), text[2] ),
			std::make_tuple( 1, 3, toU16(strlen(text[0].c_str())), text[3] ),
			std::make_tuple( 0, 4, toU16(strlen(text[0].c_str())), text[4] )
		};

		static void Writer( UDPSocket& socket, MessageArray const& messages ) {
			DataHeader header;
			for( auto msg : messages ) {
				header.messageID	= std::get<0>(msg);
				header.messageLen	= std::get<2>(msg);
				header.messageSeq	= std::get<1>(msg);
				socket.SetHeader( header );
				socket.WriteData( std::get<3>(msg).c_str(), header.messageLen );
				socket.UDPSend( sizeof(header) + header.messageLen + 1 );
			}
 		}

		static void Reader( UDPSocket& socket, TextArray& messages ) {
			static std::mutex lock;
			DataHeader const* pMsg = nullptr;
			std::string dataStr;
			std::size_t length = 0;

			do {
				length = socket.UDPReceive();
				dataStr.clear();
				dataStr = socket.ReadDataAsString();
				pMsg = socket.GetReceiveHeader();
				messages[pMsg->messageSeq] = dataStr;
				{
					std::lock_guard<std::mutex> guard(lock);

					std::ostringstream ostr;
					ostr << "Thread [" << std::this_thread::get_id() << "] received message"
						<< " id = " << pMsg->messageID
						<< " size = " << pMsg->messageLen
						<< " seqNo = " << pMsg->messageSeq
						<< " data = " << dataStr
						<< std::endl << std::ends;
					Logger::WriteMessage( ostr.str().c_str());
				}
			}while( pMsg != nullptr && pMsg->messageID != 0 );
		}

		TEST_METHOD(UDPSocketTest)
		{
			Logger::WriteMessage( "Starting UDP test" );
			UDPSocket socket1 = UDPSocket();
			UDPSocket socket2 = UDPSocket();
			socket1.CreateUDPSocket( "48000", "127.0.0.1" );
			socket1.BindSocket( 48001 );
			socket2.CreateUDPSocket( "48001", "127.0.0.1" );
			socket2.BindSocket( 48000);

			TextArray readmessages1;
			TextArray readmessages2;
			
			std::thread writer1( &UnitTest::Writer, std::ref(socket1), std::cref(messages) );
			std::thread reader1( &UnitTest::Reader, std::ref(socket1), std::ref(readmessages1) );

			std::thread writer2( &UnitTest::Writer, std::ref(socket2), std::cref(messages) );
			std::thread reader2( &UnitTest::Reader, std::ref(socket2), std::ref(readmessages2) );

			writer1.join();
			writer2.join();
			reader1.join();
			reader2.join();

			socket2.Close();
			socket1.Close();

			Assert::IsTrue(text == readmessages1);
			Assert::IsTrue(text == readmessages2);
		}
		TEST_METHOD_CLEANUP(DeinitUDPSocketTest){
			WSACleanup();
		}
	};
}
