#pragma once

//basic setting
//constexpr char* SPRITE_FILE_PATH = "Data/Images/SpriteSheetStarterKit/";
//char SPRITE_FILE_PATH[100] = "Data/Images/SpriteSheetStarterKit/";
constexpr float CLIENT_ASPECT=16.f/9.f;
constexpr int NORMAL_SCALE=1;
constexpr int NORMAL_ROTATE_DEGREES=0;
constexpr float LINE_THICK=0.05f;

// map
constexpr int MAP_SIZE_X=20;
constexpr int MAP_SIZE_Y=30;
constexpr int RANDOM_ROCK_NUM=20;
constexpr int RANDOM_MUD_NUM = 10;
constexpr int NUM_MAP = 4;

//player
constexpr float MAX_TURNING_DIFFERENCE=3.f;
constexpr float BARREL_ANGULAR_VELOCITY = 180.f;
constexpr float PLAYER_SPEED = 3.f;
constexpr float PLAYER_EXPLOSION_RADIU = 1.f;
constexpr float PLAYER_EXPLOSION_DURATION = 3.f;

//Bullet
constexpr float BULLET_SPEED = 10.f;
constexpr float BULLET_WATER_SPEED = 5.f;
constexpr float BULLET_EXPLOSION_RADIU = .3f;
constexpr float BULLET_EXPLOSION_DURATION = .5f;

//Tile
constexpr float BRICK_EXPLOSION_RADIU = .8f;
constexpr float BRICK_EXPLOSION_DURATION = .5f;


//Turret
constexpr float	TURRET_BARREL_ANGULAR_VELOCITY = 180.f;
constexpr float TURRET_VISION_DISTANCE = 15.f;
constexpr int	TURRET_HEALTH = 5;
constexpr float TURRET_MAX_SHOOT_DEGREES = 5.f;
constexpr float TURRET_SHOOT_COOL_DOWN = 1.3f;
constexpr float TURRET_EXPLOSION_RADIU = .6f;
constexpr float TURRET_EXPLOSION_DURATION = .8f;

//Tank
 constexpr float	ENEMY_TANK_ANGULAR_VELOCITY = 180.f;
 constexpr float	ENEMY_TANK_VISION_DISTANCE = 10.f;
 constexpr int		ENEMY_TANK_HEALTH = 5;
 constexpr float	ENEMY_TANK_SPEED = 1.f;
 constexpr float	ENEMY_TANK_MAX_SHOOT_DEGREES = 5.f;
 constexpr float	ENEMY_TANK_MAX_MOVE_DEGREES = 45.f;
 constexpr float	ENEMY_TANK_SHOOT_COOL_DOWN = 1.7f;
 constexpr float	ENEMY_TANK_MOVE_COOL_DOWN = 2.f;
 constexpr float	ENEMY_TANK_RAYCAST_DIST = 1.f;
 constexpr float	ENEMY_TANK_EXPLOSION_RADIU = .6f;
 constexpr float	ENEMY_TANK_EXPLOSION_DURATION = .8f;
/*
 constexpr float	ENEMY_TANK_
	 constexpr float	ENEMY_TANK_
	 constexpr float	ENEMY_TANK_
	 constexpr float	ENEMY_TANK_
	 constexpr float	ENEMY_TANK_
	 constexpr float	ENEMY_TANK_
	 constexpr float	ENEMY_TANK_
	 constexpr float	ENEMY_TANK_
	 constexpr float	ENEMY_TANK_
 constexpr float	ENEMY_TANK_
*/
 