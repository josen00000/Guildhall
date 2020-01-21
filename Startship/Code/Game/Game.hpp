#pragma once
#include<Engine/Math/vec2.hpp>
#include<Game/GameCommon.hpp>
#include<Engine/Math/RandomNumberGenerator.hpp>

class RenderContext;
class PlayerShip;
class Bullet;
class Asteroid;
class InputSystem;
class Camera;
class Beetle;
class Wasp;
class Debris;
class Entity;
class Wave;
class Game {
public:
	Game(){}
	~Game(){}
	Game(RenderContext* gameRenderer,InputSystem* inputSystem, bool isDevelop,Camera* inCamera);
	//basic

	//REVIEW: the spawn and render functions for each objects (asteroid, bullet) are
	//almost same except for array and objects. better way do it?
	void Startup();
	void Shutdown();
	void BeginFrame();
	void RunFrame(float deltaTime);
	const void Render();
	void ControlCollision();
	void RenderUI();
	void Reset();
	void UpdateCoolDown(float deltaTime);

	//wave functions
	void CreateWaves();
	void GenerateWave(int waveIndex);
	bool CheckWaveComplete();
	
	//playership functions
	void SpawnPlayerShip();
	void DestroyPlayerShip();
	void UpdateShip(float deltaTime);
	void RotatePlayerShip(int direction);
	void SpeedUpPlayerShip(bool speedUp);
	void Fire();
	void ShootFromXbox();
	void DeleteGarbageEntities();
	void RespawnFromXbox();
	
	//enemy beetle functions
	void RenderBeetles();
	void SpawnBeetle(PlayerShip& playerShip);
	void DestroyBeetle();
	void UpdateBeetles( float deltaTime );
	int FindCurrentBeetlesIndex();

	//enemy wasp functions
	void RenderWasps();
	void SpawnWasp( PlayerShip& playerShip );
	void DestroyWasp();
	void UpdateWasps( float deltaTime );
	int FindCurrentWaspssIndex();
	
	//bullet functions
	void SpawnBullet(PlayerShip& playerShip);
	void SpawnBulletWithAngleDegree(PlayerShip& playership, float degree);
	void SpawnBUlletsAll();

	void RenderBullets();
	void DestroyBullet();
	void UpdateBullets(float deltaTime);
	int FindCurrentBulletsIndex();
	

	//Asteroid functions
	void SpawnAsteroid();
	void DestroyAsteroid();
	void RenderAsteroids();
	int FindCurrentAsteroidsIndex();
	void UpdateAsteroids(float deltaTime);
	
	//Debris function
	void SpawnBulletDebris(Bullet& deadBody);
	void SpawnPlayerShipDebris(PlayerShip& deadBody);
	void SpawnAsteroidDebris(Asteroid& deadBody);
	void SpawnBeetleDebris(Beetle& deadBody);
	void SpawnWaspDebris(Wasp& deadBody);

	void DestroyDebris();
	void RenderDebris();
	int FindCurrentDebrisIndex();
	void UpdateDebris(float deltaTime);

	// Camera functions
	void UpdateCamera(float deltaTime);
	void CameraShake();

	
	
	//Input functions
	bool HandlePlayerInputPressed(unsigned char keyCode);
	bool HandlePlayerInputReleased(unsigned char keyCode);
	

	//Display Functions (need to delete)
	int DisplayMessageBoxAsteroid();
	int DisplayMessageBoxBullet();
private:
	void Update(float deltaTIme);



public:

	bool m_developMode=false;
	bool isAppQuit=false;
	bool m_enableSpawnBullet=true;
	bool m_enableSpawnAsteroid=true;
	bool m_enableSpawnBulletsAll=true;
	float yOffset=FIRE_PLACE_Y;
	float m_BulletsAllCoolDown=MAX_BULLET_ALL_COOLDOWN;
	bool m_isAttractMode=true;
	PlayerShip* m_playerShip=nullptr;
	Bullet* m_bullets[MAX_BULLETS]={};
	Asteroid* m_asteroids[MAX_ASTEROIDS]={};
	Beetle* m_beetles[MAX_BEETLES]={};
	Wasp* m_wasps[MAX_WASPS]={};
	Debris* m_debris[MAX_DEBRIS]={};
	Wave* m_waves[MAX_WAVES]={	};
	Vec2 s_leftdown=  Vec2(0.f,0.f);
	Vec2 s_rightup=  Vec2(200.f,100.f);
	RenderContext* m_theRenderer=nullptr;
	InputSystem* m_theInput=nullptr;
	Camera* m_camera=nullptr;
	RandomNumberGenerator m_rng;
	float m_screenShakeIntensity=0;
	float m_screenShakeDecreaseRate=SHAKE_DECREASE_RATE;
	int m_currentWave=0;
	int m_currentLife=4;
};