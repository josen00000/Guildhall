#pragma once
#include <thread>

class JobSystemWorkerThread {
public:
	JobSystemWorkerThread();
	~JobSystemWorkerThread();

public:
	void WorkThreadMain();

private:
	std::thread*	m_threadObject = nullptr;
};