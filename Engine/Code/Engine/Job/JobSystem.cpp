#include "JobSystem.hpp"
#include "Engine/Job/JobSystemWorkerThread.hpp"
#include "Engine/Job/Job.hpp"

void JobSystem::CreateWorkerThread()
{
	int tempThreadNum = 8;
	for( int i = 0; i < tempThreadNum; i++ ) {
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