#include "JobSystem.hpp"
#include "Engine/Job/JobSystemWorkerThread.hpp"
#include "Engine/Job/Job.hpp"
#include "Engine/Core/DevConsole.hpp"

extern DevConsole* g_theConsole;

JobSystem::JobSystem()
{

}

JobSystem::~JobSystem()
{

}

void JobSystem::CreateWorkerThread( int threadNum )
{
	
	for( int i = 0; i < threadNum; i++ ) {
		JobSystemWorkerThread* tempThread = new JobSystemWorkerThread();
		m_workerThreads.push_back( tempThread );
	}
}

void JobSystem::PostJob( Job* job )
{
	m_undoJobsMutex.lock();
	m_undoJobs.push_back( job );
	m_undoJobsMutex.unlock();
}

void JobSystem::PostRunningJob( Job* job )
{
	m_runningJobsMutex.lock();
	m_runningJobs.push_back( job );
	m_runningJobsMutex.unlock();
}

Job* JobSystem::RequestJob( )
{
	m_undoJobsMutex.lock();
	Job* job = nullptr;
	if( !m_undoJobs.empty() ) {
		job = m_undoJobs.front();
		m_undoJobs.pop_front();
		m_undoJobsMutex.unlock();
		return job;
	}
	else {
		m_undoJobsMutex.unlock();
		return nullptr;
	}
}

void JobSystem::PostCompletedJob( Job* job )
{
	m_runningJobsMutex.lock();
	for( auto iter = m_runningJobs.begin(); iter != m_runningJobs.end(); ++iter ) {
		if( (*iter)->m_jobID == job->m_jobID ) {
			m_runningJobs.erase( iter );
			break;
		}
	}
	m_runningJobsMutex.unlock();

	m_completedJobsMutex.lock();
	m_completedJobs.push_back( job );
	m_completedJobsMutex.unlock();
}

void JobSystem::ClaimAndDeleteAllCompletedJobs()
{
	std::deque<Job*> claimedJobs;

	m_completedJobsMutex.lock();
	m_completedJobs.swap( claimedJobs );
	m_completedJobsMutex.unlock();

	while( !claimedJobs.empty() ) {
		Job* tempJob = claimedJobs.front();
		claimedJobs.pop_front();
		tempJob->OnCompleteCallback();
		delete tempJob;
	}
}

void JobSystem::Shutdown()
{
	g_theConsole->DebugLog( "Shutting down the job system." );
	m_undoJobsMutex.lock();
	for( int i = 0; i < m_undoJobs.size(); i++ ) {
		g_theConsole->DebugLogf( "unfinished job: %i", m_undoJobs[i]->GetJobID() );
	}
	m_undoJobs.clear();
	m_undoJobsMutex.unlock();

	for( int i = 0; i < m_workerThreads.size(); i++ ) {
		m_workerThreads[i]->Quit();
		m_workerThreads[i]->WaitUntilFinish();
		delete m_workerThreads[i];
	}
	m_completedJobsMutex.lock();
	while( !m_completedJobs.empty() ) {
		Job* completedJob = m_completedJobs.front();
		m_completedJobs.pop_front();
		completedJob->OnCompleteCallback();
	}
	m_completedJobsMutex.unlock();
}
