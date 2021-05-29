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
constexpr float GAME_CAMERA_MIN_X	= -20.f;
constexpr float GAME_CAMERA_MIN_Y	= -11.25f;
constexpr float GAME_CAMERA_MAX_X	= 20.f;
constexpr float GAME_CAMERA_MAX_Y	= 11.25f;
constexpr float GAME_CHECK_WIDTH	= 20.f;
constexpr float GAME_CHECK_HEIGHT	= 10.f;


constexpr float UI_CAMERA_MIN_X		= 0;
constexpr float UI_CAMERA_MIN_Y		= 0;
constexpr float UI_CAMERA_MAX_X		= 200;
constexpr float UI_CAMERA_MAX_Y		= 100;

constexpr float DEV_CAMERA_MIN_X	= 0;
constexpr float DEV_CAMERA_MIN_Y	= 0;
constexpr float DEV_CAMERA_MAX_X	= 30;
constexpr float DEV_CAMERA_MAX_Y	= 20;

constexpr char MAP_DEF_FILE_PATH[]	= "Data/Definitions/Maps.xml";
constexpr char TILE_DEF_FILE_PATH[]	= "Data/Definitions/Tiles.xml";
constexpr char ACTOR_DEF_FILE_PATH[] = "Data/Definitions/Actors.xml";

// Camera System
constexpr int MAX_CAMERA_CONTROLLER_NUM = 5;

	// Camera Window
constexpr float MIN_CAMERA_WINDOW_WIDTH			= 0.1f;
constexpr float MAX_CAMERA_WINDOW_WIDTH			= 40.f;
constexpr float MIN_CAMERA_WINDOW_HEIGHT		= 0.1f;
constexpr float MAX_CAMERA_WINDOW_HEIGHT		= 22.5f;
constexpr float DEFAULT_CAMERA_WINDOW_WIDTH		= 12.f;
constexpr float DEFAULT_CAMERA_WINDOW_HEIGHT	= 8.f;

	// Camera frame
constexpr float DEFAULT_FORWARD_VELOCITY_FRAME_DIST = 1.5f;
constexpr float DEFAULT_AIM_FRAME_DIST				= 1.5f;
constexpr float MAX_FORWARD_VELOCITY_FRAME_DIST		= 10.f;
constexpr float MAX_AIM_FRAME_DIST					= 10.f;

	// Camera Shake
constexpr float DEFAULT_MAX_POSITION_SHAKE_DISTANCE			= 10.f;
constexpr float DEFAULT_MAX_ROTATION_SHAKE_DEGREE			= 10.f;