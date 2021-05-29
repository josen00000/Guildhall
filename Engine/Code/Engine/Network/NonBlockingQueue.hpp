#pragma once

#include "Engine/Core/ErrorWarningAssert.hpp"
#include <queue>
#include <atomic>


template<typename T>
class NonBlockingQueue : protected std::queue<T> {
	using value_type = typename T;
	using base = typename std::queue<T>;

public:
	NonBlockingQueue(): base(){}
	~NonBlockingQueue() = default;
	NonBlockingQueue( NonBlockingQueue const& copyFrom ) = delete;
	NonBlockingQueue( NonBlockingQueue const&& moveFrom ) = delete;
	
	NonBlockingQueue& operator=( NonBlockingQueue const& ) = delete;
	NonBlockingQueue& operator=( NonBlockingQueue const&& ) = delete;


	void Push( const value_type& value );
	bool Empty() const{ return base::empty(); }
	value_type Pop();
	std::vector<value_type> PopAll();

protected:
	void Lock();
	void Unlock();
private:
	const int UNLOCKED = 0;
	const int LOCKED = 1;
	std::atomic<int> m_atomicLock;
};

template<typename T>
std::vector<T> NonBlockingQueue<T>::PopAll()
{
	std::vector<T> result;
	Lock();
	while( !base::empty() ) {
		T value = base::front();
		base::pop();
		result.push_back( value );
	}
	Unlock();
	return result;
}

template<typename T>
void NonBlockingQueue<T>::Lock()
{
	int expected = UNLOCKED;
	while( !m_atomicLock.compare_exchange_strong( expected, LOCKED ) );
}
template<typename T>
void NonBlockingQueue<T>::Unlock()
{
	int expected = LOCKED;
	while( !m_atomicLock.compare_exchange_strong( expected, UNLOCKED ) );
}


template<typename T>
T NonBlockingQueue<T>::Pop()
{
	value_type value = value_type();
	Lock();
	if( !base::empty() ) {
		value = base::front();
		base::pop();
	}
	Unlock();
	return value;
}

template<typename T>
void NonBlockingQueue<T>::Push( const value_type& value )
{
	Lock();
	base::push( value );
	Unlock();
}
