#include "JobSystemWorkerThread.hpp"

JobSystemWorkerThread::JobSystemWorkerThread()
{
	m_threadObject = new std::thread( &JobSystemWorkerThread::WorkThreadMain );
}

JobSystemWorkerThread::~JobSystemWorkerThread()
{
	delete m_threadObject;
	m_threadObject = nullptr;
}

void JobSystemWorkerThread::WorkThreadMain( )
{
// 	printf( "start work in thread " );
// 	while( true ) {
// 
// 	}
}
