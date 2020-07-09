#pragma once
#include "Engine/Core/Time/Clock.hpp"

class Timer {
public:
	void SetSeconds( Clock* clock, double timeToWait );
	void SetSeconds( double timeToWait );

	void Reset();
	void Stop();

	double GetElapsedSeconds() const;
	double GetSecondsRemaining() const;

	bool HasElapsed() const;
	bool CheckAndDecrement();
	int CheckAndDecrementAll();
	bool CheckAndReset();

	bool isRunning() const;

public:
	bool m_isStop = false;
	double m_startSeconds = 0.0;
	double m_durationSeconds = -1.0;
	Clock* m_clock;
};