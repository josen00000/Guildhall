#pragma once
#include "Engine/Platform/Window.hpp"

extern Window* g_theWindow;

//basic setting
constexpr float CLIENT_ASPECT		= 16.f / 9.f;
constexpr int NORMAL_SCALE			= 1;
constexpr int NORMAL_ROTATE_DEGREES	= 0;
constexpr float LINE_THICK			= 0.1f;

// map
constexpr int MAP_SIZE_X = 20;
constexpr int MAP_SIZE_Y = 30;
constexpr int RANDOM_ROCK_NUM = 20;
constexpr int RANDOM_MUD_NUM = 10;
constexpr int NUM_MAP = 4;
constexpr float MINIMUM_HEAT_RANGE = -99999999.f;
constexpr float	MAXIMUM_HEAT_RANGE =  99999999.f;


//Tile
constexpr float BRICK_EXPLOSION_RADIU = .8f;
constexpr float BRICK_EXPLOSION_DURATION = .5f;

//Camera
constexpr float GAME_CAMERA_MIN_X	= -8;
constexpr float GAME_CAMERA_MIN_Y	= -4.5;
constexpr float GAME_CAMERA_MAX_X	= 8;
constexpr float GAME_CAMERA_MAX_Y	= 4.5;

constexpr float UI_CAMERA_MIN_X		= 0;
constexpr float UI_CAMERA_MIN_Y		= 0;
constexpr float UI_CAMERA_MAX_X		= 200;
constexpr float UI_CAMERA_MAX_Y		= 100;