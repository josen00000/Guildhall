#pragma once
#include <vector>

class Clock{
public:
	Clock();
	Clock( Clock* parent );
	~Clock();

	void Update( double deltaSeconds );
	void Reset();

	// Control
	void Pause();
	void Resume();
	void SetScale( double scale );

	// Accessor
	double GetTotalSeconds() const { return m_totalTime; }
	double GetLastDeltaSeconds() const { return m_deltaTime; }

	double GetScale() const { return m_scale; }
	bool IsPaused() const { return m_isPause; }

	// Help
	void SetFrameLimits( double minFrameTime, double maxFrameTime );

public:
	void SetParent( Clock* parent );
	void AddChild( Clock* child );

public:
	// need a way to...
	// ...current time state (current time, delta, etc...)
	// ...track pause state
	// ...track scale
	// ...know my parent
	// ...know my children
	// 

public:
	static void SystemStartUp();
	static void SystemShutDown();
	static void BeginFrame();

	static Clock* GetMaster();

public:
	bool m_isPause = false;
	double m_totalTime = 0;
	double m_deltaTime = 0;
	double m_scale = 1;
	Clock* m_parent = nullptr;
	std::vector<Clock*> m_children;
};