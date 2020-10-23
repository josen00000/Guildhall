#pragma once
#include "Game/Game.hpp"

class MultiPlayerGame : protected Game {
	MultiPlayerGame() = default;
	~MultiPlayerGame() = default;
	MultiPlayerGame( MultiPlayerGame const& ) = default;
	MultiPlayerGame( MultiPlayerGame &&  ) = default;

};