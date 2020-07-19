#pragma once
#include <thread>

class JobSystemWorkerThread {
public:
	JobSystemWorkerThread();
	~JobSystemWorkerThread();

public:
	void WorkThreadMain( int threadID );
	void WaitUntilFinish();
	void Quit();
	void Destroy();

private:
	std::thread*	m_threadObject	= nullptr;
	int				m_threadID		= 0;
	bool			m_isQuit		= false;
};