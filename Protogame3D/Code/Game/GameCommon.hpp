#pragma once
#include "Engine/Platform/Window.hpp"

extern Window* g_theWindow;

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
constexpr float GAME_CAMERA_MIN_X	= -8;
constexpr float GAME_CAMERA_MIN_Y	= -4.5;
constexpr float GAME_CAMERA_MAX_X	= 8;
constexpr float GAME_CAMERA_MAX_Y	= 4.5;

constexpr float UI_CAMERA_MIN_X		= 0;
constexpr float UI_CAMERA_MIN_Y		= 0;
constexpr float UI_CAMERA_MAX_X		= 200;
constexpr float UI_CAMERA_MAX_Y		= 100;