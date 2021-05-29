#pragma once
#include "Game/Game.hpp"

class Camera;

class SinglePlayerGame : public Game {
public:
	SinglePlayerGame() = default;
	~SinglePlayerGame() = default;
	explicit SinglePlayerGame( Camera* gameCamera, Camera* UICamera );
	SinglePlayerGame( SinglePlayerGame const& ) = default;
	SinglePlayerGame( SinglePlayerGame &&  ) = default;

};