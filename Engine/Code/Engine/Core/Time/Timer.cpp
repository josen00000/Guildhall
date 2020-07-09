#include "Engine/Core/Time/Timer.hpp"


void Timer::SetSeconds( Clock* clock, double timeToWait )
{
	m_clock = clock;
	m_startSeconds = m_clock->GetTotalSeconds();
	m_durationSeconds = timeToWait;
}

void Timer::SetSeconds( double timeToWait )
{
	m_clock = Clock::GetMaster();
	m_startSeconds = m_clock->GetTotalSeconds();
	m_durationSeconds = timeToWait;
}

void Timer::Reset()
{
	m_startSeconds = m_clock->GetTotalSeconds();
	m_durationSeconds = -1.0;
}

void Timer::Stop()
{
	// what we do in stop
	m_clock->Pause();
}

double Timer::GetElapsedSeconds() const
{
	double currentTime = m_clock->GetTotalSeconds();
	double elapsedSeconds = currentTime - ( m_startSeconds + m_durationSeconds );
	return elapsedSeconds;
}

double Timer::GetSecondsRemaining() const
{
	double currentTime = m_clock->GetTotalSeconds();
	double remainingSeconds = (m_startSeconds + m_durationSeconds) - currentTime;
	return remainingSeconds;
}

bool Timer::HasElapsed() const
{
	if( m_durationSeconds < 0 ){ return false; }
	double currentTime = m_clock->GetTotalSeconds();
	return ( currentTime >= ( m_startSeconds + m_durationSeconds) );
}

bool Timer::CheckAndDecrement()
{
	if( HasElapsed() ){
		m_startSeconds += m_durationSeconds;
		return true;
	}
	else{
		return false;
	}
}

int Timer::CheckAndDecrementAll()
{
	int index = 0;
	while( CheckAndDecrement() ){
		index++;
	}
	return index;
}

bool Timer::CheckAndReset()
{
	if( HasElapsed() ) {
		m_startSeconds = m_clock->GetTotalSeconds();
		return true;
	}
	else {
		return false;
	}
}

bool Timer::isRunning() const
{
	double dt = m_clock->GetLastDeltaSeconds();
	if( dt > 0 ){
		return true;
	}
	else{
		return false;
	}
}
