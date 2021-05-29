#pragma once

#include "Engine/Core/ErrorWarningAssert.hpp"
#include <queue>
#include <atomic>


template<typename T>
class NonBlockingPriorityQueue : protected std::priority_queue<T> {
	using value_type = typename T;
	using base = typename std::priority_queue<T>;

public:
	NonBlockingPriorityQueue(): base(){}
	~NonBlockingPriorityQueue() = default;
	NonBlockingPriorityQueue( NonBlockingPriorityQueue const& copyFrom ) = delete;
	NonBlockingPriorityQueue( NonBlockingPriorityQueue const&& moveFrom ) = delete;
	
	NonBlockingPriorityQueue& operator=( NonBlockingPriorityQueue const& ) = delete;
	NonBlockingPriorityQueue& operator=( NonBlockingPriorityQueue const&& ) = delete;


	void push( const value_type& value );
	bool empty();
	value_type top();
	void pop();
	//std::vector<value_type> GetAllElements();

protected:
	void Lock();
	void Unlock();
private:
	const int UNLOCKED = 0;
	const int LOCKED = 1;
	std::atomic<int> m_atomicLock;
};

template<typename T>
T NonBlockingPriorityQueue<T>::top()
{
	Lock();
	T tempValue = base::top();
	Unlock();
	return tempValue;
}

template<typename T>
bool NonBlockingPriorityQueue<T>::empty()
{
	Lock();
	bool result = base::empty();
	Unlock();
	return result;
}


template<typename T>
void NonBlockingPriorityQueue<T>::Lock()
{
	int expected = UNLOCKED;
	while( !m_atomicLock.compare_exchange_strong( expected, LOCKED ) );
}
template<typename T>
void NonBlockingPriorityQueue<T>::Unlock()
{
	int expected = LOCKED;
	while( !m_atomicLock.compare_exchange_strong( expected, UNLOCKED ) );
}


template<typename T>
void NonBlockingPriorityQueue<T>::pop()
{
	Lock();
	if( !base::empty() ) {
		base::pop();
	}
	Unlock();
}

template<typename T>
void NonBlockingPriorityQueue<T>::push( const value_type& value )
{
	Lock();
	base::push( value );
	Unlock();
}
