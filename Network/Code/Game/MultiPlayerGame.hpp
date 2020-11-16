#pragma once
#include "Game/Game.hpp"

class Camera;

class MultiPlayerGame : public Game {
public:
	MultiPlayerGame() = default;
	~MultiPlayerGame() = default;
	explicit MultiPlayerGame ( Camera* gameCamera, Camera* UICamera );
	MultiPlayerGame( MultiPlayerGame const& ) = default;
	MultiPlayerGame( MultiPlayerGame &&  ) = default;
};