#pragma once

class Job {
public:
	Job();
	virtual ~Job(){}
	virtual void Execute() = 0;
	virtual void OnCompleteCallback() = 0;
	int GetJobID() const { return m_jobID; }

public:
	int m_jobID = 0;
};