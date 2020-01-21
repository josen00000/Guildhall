#pragma once

//constexpr float CLIENT_ASPECT=2.0f;
//frame setting const
constexpr float FRAME_NORMAL_DELTA_SECOND=0.016f;
constexpr float FRAME_PAUSED_DELTA_SECOND=0.f;
constexpr float FRAME_SLOW_DELTA_SECOND=0.0016f;
//basic setting
constexpr int NORMAL_SCALE=1;
constexpr int NORMAL_ROTATE_DEGREES=0;
constexpr float LINE_THICK=0.1f;

//player ship setting
constexpr float FIRE_PLACE_X=1.f;
constexpr float FIRE_PLACE_Y=1.f;
constexpr int SHIP_VERTEX_NUM=15;
constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;
constexpr int PLAYER_SHIP_DEBRIS_NUM=15;
//asteroid setting
constexpr int NUM_STARTING_ASTEROIDS = 6;
constexpr int MAX_ASTEROIDS = 12;
constexpr float ASTEROID_SPEED = 10.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float ASTEROID_COSMETIC_RADIUS = 2.0f;
constexpr int ASTEROID_VERTEX_NUM=48;
constexpr int ASTEROID_DEBRIS_NUM=10;

//bullet setting
constexpr int MAX_BULLETS = 200;
constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 50.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;
constexpr int BULLET_VERTEX_NUM=6;
constexpr int BULLET_DEBRIS_NUM=3;
constexpr float MAX_BULLET_ALL_COOLDOWN=5.f;

//camera shake settings
constexpr float MAX_CAMERA_SHAKE_VALUE=2.0f;
constexpr float MAX_CAMERA_SHAKE_INTENSITY=1.0f;
constexpr float SHAKE_DECREASE_RATE=.5f;
constexpr float WORLD_SIZE_X = 200.f;
constexpr float WORLD_SIZE_Y = 100.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

// beetle setting
constexpr float BEETLE_SPEED=10.f;
constexpr int BEETLE_VERTEX_NUM=15;
constexpr int BEETLE_HEALTH=3;
constexpr float BEETLE_PHYSICS_RADIUS = 1.6f;
constexpr float BEETLE_COSMETIC_RADIUS = 2.0f;
constexpr int MAX_BEETLES=20;
constexpr float BEETLE_SPAWN_POSITION_X=205.f;
constexpr float BEETLE_SPAWN_POSITION_Y=80.f;
constexpr int BEETLE_DEBRIS_NUM=8;

//wasp setting
constexpr float WASP_ACCELERATION=20.f;
constexpr int WASP_VERTEX_NUM=15;
constexpr int WASP_HEALTH=3;
constexpr float WASP_PHYSICS_RADIUS = 1.6f;
constexpr float WASP_COSMETIC_RADIUS = 2.0f;
constexpr float WASP_MAX_SPEED=10.f;
constexpr int MAX_WASPS=20;
constexpr float WASP_SPAWN_POSITION_X=0.f;
constexpr float WASP_SPAWN_POSITION_Y=80.f;
constexpr int WASP_DEBRIS_NUM=6;


//debri setting
constexpr int MAX_DEBRIS=50;
constexpr int DEBRIS_VERTEX_NUM=48;
constexpr float DEBRIS_FADE_RATE=0.5f;
constexpr float DEBRIS_SPEED=3.f;

//Wave setting
constexpr int MAX_WAVES=5;
//wave1
constexpr int WAVE1_ASTEROID_NUM=5;
constexpr int WAVE1_BEETLE_NUM=1;
constexpr int WAVE1_WASP_NUM=2;
//wave2
constexpr int WAVE2_ASTEROID_NUM=7;
constexpr int WAVE2_BEETLE_NUM=2;
constexpr int WAVE2_WASP_NUM=3;
//wave3
constexpr int WAVE3_ASTEROID_NUM=8;
constexpr int WAVE3_BEETLE_NUM=3;
constexpr int WAVE3_WASP_NUM=4;
//wave4
constexpr int WAVE4_ASTEROID_NUM=9;
constexpr int WAVE4_BEETLE_NUM=4;
constexpr int WAVE4_WASP_NUM=5;
//wave5
constexpr int WAVE5_ASTEROID_NUM=10;
constexpr int WAVE5_BEETLE_NUM=5;
constexpr int WAVE5_WASP_NUM=6;