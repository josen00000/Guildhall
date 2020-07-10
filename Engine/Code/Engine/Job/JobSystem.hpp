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
	void CreateWorkerThread();
	void PostJob( Job* job );
	void ClaimAndDeleteAllCompletedJobs();

private:
	std::deque<Job*>	m_undoJobs;
	std::deque<Job*>	m_completedJobs;
	std::mutex			m_undoJobsMutex;
	std::mutex			m_completedJobsMutex;

	std::vector<JobSystemWorkerThread*>		m_workerThreads;
};