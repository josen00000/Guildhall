#include "Job.hpp"

Job::Job()
{
	static int jobID = 0;
	m_jobID = jobID;
	jobID++;
}
