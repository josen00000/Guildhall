#pragma once
#include "Game/Game.hpp"

class SinglePlayerGame : public Game {
	SinglePlayerGame() = default;
	~SinglePlayerGame() = default;
	SinglePlayerGame( SinglePlayerGame const& ) = default;
	SinglePlayerGame( SinglePlayerGame &&  ) = default;

};