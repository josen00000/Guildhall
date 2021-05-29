#pragma once
#include "Engine/Core/EngineCommon.hpp"

class Game;

class Coroutine {
public:
	Coroutine(){}
	virtual ~Coroutine();

	virtual void Execute() = 0;
	virtual void Yield();

private:
	// original function pointer
	//Goto
};

