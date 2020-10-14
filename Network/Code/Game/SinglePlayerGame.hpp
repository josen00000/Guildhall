#pragma once
#include "Game/Game.hpp"

class SinglePlayerGame : protected Game {
	SinglePlayerGame() = default;
	~SinglePlayerGame();
	SinglePlayerGame( SinglePlayerGame const& ) = default;
	SinglePlayerGame( SinglePlayerGame &&  ) = default;

};