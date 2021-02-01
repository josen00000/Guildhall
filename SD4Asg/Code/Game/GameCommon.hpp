#pragma once
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"

class Game;

extern Window*			g_theWindow;
extern RenderContext*	g_theRenderer;
extern Game*			g_theGame;

enum RotateDirection
{
	ROTATE_LEFT,
	ROTATE_RIGHT
};


//basic setting
constexpr float CLIENT_ASPECT		= 16.f / 9.f;
constexpr int NORMAL_SCALE			= 1;
constexpr int NORMAL_ROTATE_DEGREES	= 0;
constexpr float LINE_THICK			= 0.1f;

//Map
constexpr int MAP_SIZE_X		= 20;
constexpr int MAP_SIZE_Y		= 30;
constexpr int RANDOM_ROCK_NUM	= 20;


//Player
constexpr float MAX_TURNING_DIFFERENCE = 3.f;


//Camera
constexpr float GAME_CAMERA_MIN_X	= 0;
constexpr float GAME_CAMERA_MIN_Y	= 0;
constexpr float GAME_CAMERA_MAX_X	= 160;
constexpr float GAME_CAMERA_MAX_Y	= 90;

constexpr float UI_CAMERA_MIN_X		= 0;
constexpr float UI_CAMERA_MIN_Y		= 0;
constexpr float UI_CAMERA_MAX_X		= 200;
constexpr float UI_CAMERA_MAX_Y		= 100;

constexpr float DEV_CAMERA_MIN_X	= 0;
constexpr float DEV_CAMERA_MIN_Y	= 0;
constexpr float DEV_CAMERA_MAX_X	= 30;
constexpr float DEV_CAMERA_MAX_Y	= 20;


// GameObject
constexpr float ROTATE_SPEED		= 60.f;