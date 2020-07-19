#pragma once
#include <queue>
#include <mutex>

class Job;
class JobSystemWorkerThread;

class JobSystem {
public:
	JobSystem();
	~JobSystem();

public:
	// Accessor
	int GetUndoJobsNum() const { return (int)m_undoJobs.size(); }
	int GetRunningJobsNum() const { return (int)m_runningJobs.size(); }
	int GetCompletedJobsNum() const { return (int)m_completedJobs.size(); }

	void CreateWorkerThread( int threadNum );
	void PostJob( Job* job );
	void PostRunningJob( Job* job );
	void PostCompletedJob( Job* job ); 
	Job* RequestJob();
	void ClaimAndDeleteAllCompletedJobs();
	void Shutdown();

private:
	std::deque<Job*>	m_undoJobs;
	std::deque<Job*>	m_runningJobs;
	std::deque<Job*>	m_completedJobs;
	std::mutex			m_undoJobsMutex;
	std::mutex			m_runningJobsMutex;
	std::mutex			m_completedJobsMutex;

	std::vector<JobSystemWorkerThread*>		m_workerThreads;
};