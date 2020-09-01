#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"

static Clock* g_masterClock;

Clock::Clock()
{
	SetParent( g_masterClock );
	g_masterClock->AddChild( this );
}

Clock::Clock( Clock* parent )
{
	m_parent = parent;
	if( m_parent != nullptr ){
		parent->AddChild( this );
	}
}

Clock::~Clock()
{
	if( m_parent == nullptr ){ return; }

	for( int i = 0; i < m_children.size(); i++ ) {
		Clock* tempClock = m_children[i];
		m_parent->AddChild( tempClock );
	}
}

void Clock::Update( double deltaSeconds )
{
	if( IsPaused() ){
		deltaSeconds = 0;
	}
	else{
		deltaSeconds *= m_scale;
	}
	m_deltaTime = deltaSeconds;
	m_totalTime += m_deltaTime;
	for( int i = 0; i < m_children.size(); i++ ){
		m_children[i]->Update( m_deltaTime );
	}
}

void Clock::Reset()
{
	m_isPause = false;
	m_scale = 1;
	m_totalTime = 0;
	m_deltaTime = 0;
}

void Clock::SelfBeginFrame()
{
	static double timePreviousFrame = GetCurrentTimeSeconds();
	double timeThisFrame = GetCurrentTimeSeconds();

	double dt = timeThisFrame - timePreviousFrame;
	timePreviousFrame = timeThisFrame;
	Update( dt );
}

void Clock::Pause()
{
	m_isPause = true;
}

void Clock::Resume()
{
	m_isPause = false;
}

void Clock::SetScale( double scale )
{
	m_scale = scale;
}


void Clock::SetFrameLimits( double minFrameTime, double maxFrameTime )
{
	UNUSED(minFrameTime);
	UNUSED(maxFrameTime);
}

void Clock::SetParent( Clock* parent )
{
	m_parent = parent;
}

void Clock::AddChild( Clock* child )
{
	m_children.push_back( child );
}

void Clock::SystemStartUp()
{
	g_masterClock = new Clock( nullptr );
	g_masterClock->Reset();
}

void Clock::SystemShutDown()
{
	delete g_masterClock;
}

void Clock::BeginFrame()
{
	static double timePreviousFrame = GetCurrentTimeSeconds();
	double timeThisFrame = GetCurrentTimeSeconds();

	double dt = timeThisFrame - timePreviousFrame;
	timePreviousFrame = timeThisFrame;
	g_masterClock->Update( dt );
}

Clock* Clock::GetMaster()
{
	return g_masterClock;
}

float Clock::GetMasterDeltaSeconds()
{
	return (float)g_masterClock->GetLastDeltaSeconds();
}
