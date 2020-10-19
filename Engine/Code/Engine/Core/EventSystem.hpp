#pragma once
#include <vector>
#include <map>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/Delegate.hpp"
#include "Engine/Core/ConsumableDelegate.hpp"
#include "Engine/Core/Time/Timer.hpp"

typedef NamedProperties EventArgs; 
typedef bool ( *EventCallbackFunctionPtr )( EventArgs& args );
typedef std::multimap<std::string, std::vector<EventCallbackFunctionPtr>>::iterator registeredEventIterator;
typedef std::vector<EventCallbackFunctionPtr> eventFunctionVector;
typedef eventFunctionVector::iterator eventFunctionVectorIterator;

struct TimeDelegate {
	Timer* timer = nullptr;
	ConsumableDelegate<EventArgs&>* delegate = nullptr;
};

class EventSystem
{
public:
	void Update();
	void SubscribeEvent( const std::string& event, const EventCallbackFunctionPtr callbackFunction );
	template<typename OBJ_TYPE>
	void SubscribeMethodToEvent( const std::string& event, OBJ_TYPE* obj, bool(OBJ_TYPE::* mcb)( EventArgs& ) );
	template<typename OBJ_TYPE>
	void SetTimerByFunction( float duration, OBJ_TYPE* obj, bool(OBJ_TYPE::* mcb)( EventArgs& ) );
	void UnscribeEvent( const std::string& event, const EventCallbackFunctionPtr callbackFunction );
	void FireEvent( const std::string& event, EventArgs& args );
	std::map< std::string, ConsumableDelegate<EventArgs&>* > m_registeredEvents;
	std::vector< TimeDelegate > m_timerRegisteredEvents;
	//std::map<std::string,Delegate<EventArgs&>> m_eventCallbacks;
};

template<typename OBJ_TYPE>
void EventSystem::SubscribeMethodToEvent( const std::string& event, OBJ_TYPE* obj, bool(OBJ_TYPE::*callbackFunction)( EventArgs& ) )
{
	if( m_registeredEvents.count( event ) == 0 ) {
		m_registeredEvents[event] = new ConsumableDelegate<EventArgs&>();
	}
	ConsumableDelegate<EventArgs&>* temDelegate = m_registeredEvents[event];
	temDelegate->subscribe_method( obj,  callbackFunction );
}

template<typename OBJ_TYPE>
void EventSystem::SetTimerByFunction( float duration, OBJ_TYPE* obj, bool(OBJ_TYPE::* mcb)(EventArgs&) )
{
	TimeDelegate tempTimeDelegate{};
	tempTimeDelegate.timer = new Timer();
	tempTimeDelegate.delegate = new ConsumableDelegate<EventArgs&>();
	tempTimeDelegate.delegate->subscribe_method( obj, mcb );
	tempTimeDelegate.timer.SetSeconds( duration );
	m_timerRegisteredEvents.push_back( tempTimeDelegate );
}

