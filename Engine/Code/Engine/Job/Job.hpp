#pragma once

class Job {
public:
	Job();
	virtual ~Job(){}
	virtual void Execute() = 0;
	virtual void OnCompleteCallback() = 0;

public:
	int m_jobID = 0;
};