#include "EventSystem.hpp"
#include "Engine/Core/Error/ErrorWarningAssert.hpp"

void EventSystem::SubscribeEvent( const std::string& event, const EventCallbackFunctionPtr callbackFunction )
{
	if( m_registeredEvents.count( event ) == 0 ) {
		m_registeredEvents[event] = new ConsumableDelegate<EventArgs&>();
	}
	ConsumableDelegate<EventArgs&>* temDelegate = m_registeredEvents[event];
	temDelegate->subscribe( callbackFunction );
}

void EventSystem::UnscribeEvent( const std::string& event, const EventCallbackFunctionPtr callbackFunction )
{
	if( m_registeredEvents.count( event ) == 0 ){
		ERROR_AND_DIE( "Trying to unscribe the event not exist!" );
		return;
	}

	ConsumableDelegate<EventArgs&>* temDelegate = m_registeredEvents[event];
	temDelegate->unsubscribe( callbackFunction );

// 	eventFunctionVector tempEventVector = m_registeredEvents[event];
// 	for( eventFunctionVectorIterator it = tempEventVector.begin(); it != tempEventVector.end(); ++it ){
// 		if( *it == callbackFunction ){
// 			*it = nullptr;
// 		}
// 	}
}

void EventSystem::FireEvent( const std::string& event, EventArgs& args )
{
	if( m_registeredEvents.count( event ) == 0 ) {
		ERROR_AND_DIE( "Trying to fire the event not exist!" );
		return;
	}
	ConsumableDelegate<EventArgs&>* temDelegate = m_registeredEvents[event];
	temDelegate->invoke( args );
}


