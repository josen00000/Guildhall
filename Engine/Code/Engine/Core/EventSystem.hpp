#pragma once
#include <vector>
#include <map>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"

typedef NamedStrings EventArgs; 
typedef bool (*EventCallbackFunctionPtr)( EventArgs& args );
typedef std::multimap<std::string, std::vector<EventCallbackFunctionPtr>>::iterator registeredEventIterator;
typedef std::vector<EventCallbackFunctionPtr> eventFunctionVector;
typedef eventFunctionVector::iterator eventFunctionVectorIterator;

class EventSystem
{
public:
	void SubscribeTheEvent( const std::string& event, const EventCallbackFunctionPtr callbackFunction );
	void UnscribeTheEvent( const std::string& event, const EventCallbackFunctionPtr callbackFunction );
	void FireTheEvent( const std::string& event, EventArgs& args );
	std::map< std::string, std::vector<EventCallbackFunctionPtr> > m_registeredEvents;
};

