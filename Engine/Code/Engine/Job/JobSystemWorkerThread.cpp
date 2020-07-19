#include "JobSystemWorkerThread.hpp"
#include "Engine/Job/JobSystem.hpp"
#include "Engine/Job/Job.hpp"

extern JobSystem* g_theJobSystem;

JobSystemWorkerThread::JobSystemWorkerThread()
{
	static int threadID = 0;
	m_threadID = threadID;
	threadID++;
	m_threadObject = new std::thread( &JobSystemWorkerThread::WorkThreadMain, this, m_threadID );
}

JobSystemWorkerThread::~JobSystemWorkerThread()
{
	delete m_threadObject;
	m_threadObject = nullptr;
}

void JobSystemWorkerThread::WorkThreadMain( int threadID  )
{
	printf( "start work in thread %i ", threadID );
	while( !m_isQuit ) {
		Job* requestedJob = g_theJobSystem->RequestJob(  );
		if( requestedJob ) {
			g_theJobSystem->PostRunningJob( requestedJob );
			requestedJob->Execute();
			g_theJobSystem->PostCompletedJob( requestedJob );
		}
		else {
			std::this_thread::sleep_for( std::chrono::microseconds(10) );
		}
	}
}

void JobSystemWorkerThread::WaitUntilFinish()
{
	m_threadObject->join();
}

void JobSystemWorkerThread::Quit()
{
	m_isQuit = true;
}

void JobSystemWorkerThread::Destroy()
{
	m_isQuit = true;
	m_threadObject->join();
	delete this;
}

