#include "EventSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

void EventSystem::SubscribeTheEvent( const std::string& event, const EventCallbackFunctionPtr callbackFunction )
{
	if( m_registeredEvents.count( event ) == 0 ){
		m_registeredEvents[event] = std::vector<EventCallbackFunctionPtr>();
		m_registeredEvents[event].push_back( callbackFunction );
	}
	else{
		eventFunctionVector tempEventVector = m_registeredEvents[event];
		for( eventFunctionVectorIterator it = tempEventVector.begin(); it != tempEventVector.end(); ++it ){
			if( *it != nullptr){
				continue;
			}
			*it = callbackFunction;
			return;
		}
		tempEventVector.push_back( callbackFunction );
	}
}

void EventSystem::UnscribeTheEvent( const std::string& event, const EventCallbackFunctionPtr callbackFunction )
{
	if( m_registeredEvents.count( event ) == 0 ){
		ERROR_AND_DIE( "Trying to unscribe the event not exist!" );
		return;
	}
	
	eventFunctionVector tempEventVector = m_registeredEvents[event];
	for( eventFunctionVectorIterator it = tempEventVector.begin(); it != tempEventVector.end(); ++it ){
		if( *it == callbackFunction ){
			*it = nullptr;
		}
	}
}

void EventSystem::FireTheEvent( const std::string& event, EventArgs& args )
{
	if( m_registeredEvents.count( event ) == 0 ) {
		ERROR_AND_DIE( "Trying to fire the event not exist!" );
		return;
	}

	eventFunctionVector tempEventVector = m_registeredEvents[event];
	for( eventFunctionVectorIterator it = tempEventVector.begin(); it != tempEventVector.end(); ++it){
		EventCallbackFunctionPtr tempEventFunctionPtr = *it;
		if( tempEventFunctionPtr == nullptr ){ continue; }
		tempEventFunctionPtr( args );
	}
}


