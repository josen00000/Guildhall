#pragma once

#include <queue>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <mutex>
#include <condition_variable>


template<typename T>
class BlockingQueue : protected std::queue<T> {
	using value_type = typename T;
	using base = typename std::queue<T>;

public:
	BlockingQueue(): base(){}
	~BlockingQueue(){}
	BlockingQueue( BlockingQueue const& copyFrom ) = delete;
	BlockingQueue( BlockingQueue const&& moveFrom ) = delete;

	BlockingQueue& operator=( BlockingQueue const&& ) = delete;
	BlockingQueue& operator=( BlockingQueue const& ) = delete;


	// declaration
	void Push( const value_type& value );
	value_type Pop();
	void StopBlocking();


	// definition
// 	template<typename T>
// 	void Push( const value_type& value )
// 	{
// 		base::push( value );
// 	}

// 	template<typename T>
// 	value_type Pop()
// 	{
// 		if( !base::empty() ) {
// 			value_type value = base::front();
// 			base::pop();
// 			return value;
// 		}
// 		ERROR_RECOVERABLE( "No element in blockingQueue" );
// 		return value_type();
// 	}

protected:
private:
	std::mutex m_mutex;
	std::condition_variable m_condition;
};

template<typename T>
void BlockingQueue<T>::StopBlocking()
{
	m_condition.notify_all();
}

template<typename T>
T BlockingQueue<T>::Pop()
{
	std::unique_lock<std::mutex> uniqueLock( m_mutex );
	if( base::empty() ) {
		m_condition.wait( uniqueLock );
	}
	
	value_type value = value_type();
	if( !base::empty() ) {
		value= base::front();
		base::pop();
	}
	return value;
}

template<typename T>
void BlockingQueue<T>::Push( const value_type& value )
{
	std::lock_guard<std::mutex> guard{ m_mutex };
	base::push( value );
	m_condition.notify_all();
}
