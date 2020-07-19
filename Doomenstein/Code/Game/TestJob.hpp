#pragma once
#include <vector>
#include "Engine/Job/Job.hpp"

class TestJob : public Job {
public:
	TestJob(){}
	~TestJob(){}

public:
	virtual void Execute() override;
	virtual void OnCompleteCallback() override;

public:
	int m_testingSize = 20000;
	std::vector<int> m_data;
};