#include "TestJob.hpp"
#include <limits>
#include "Engine/Core/DevConsole.hpp"

extern DevConsole* g_theConsole;


void TestJob::Execute()
{
	m_data.resize( m_testingSize );
	for( int i = 0; i < m_data.size(); i++ ) {
		m_data[i] = i;
	}
	std::vector<int> sortedArray;
	sortedArray.reserve( m_data.size() );
	while( m_data.size() > 0 ){
		int max = m_data[0];
		int maxIndex = 0;
		
		for( int i = 0; i < m_data.size(); i++ ) {
			if( m_data[i] > max ) {
				max = m_data[i];
				maxIndex = i;
			}
		}
		
		sortedArray.push_back( max );
		m_data.erase( m_data.begin() + maxIndex );
	}

	m_data.swap( sortedArray );
}

void TestJob::OnCompleteCallback()
{
	if( m_testingSize - 1 == m_data[0] ) {
		g_theConsole->DebugLogf( "Job finished: %i", m_jobID );
	}
	else {
		g_theConsole->DebugLogf( "Job failed: %i", m_jobID );
	}
}
