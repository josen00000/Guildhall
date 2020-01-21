#include <windows.h>
#include "Game.hpp"
#include "Game/Wave.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Beetle.hpp"
#include "Game/Wasp.hpp"
#include "Game/Debris.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"

/*
Question 1 
Why the reference of return function does not work?
m_playerShip=new PlayerShip( this, Vec2( WORLD_CENTER_X, WORLD_CENTER_Y ), &m_theInput->GetXboxController( 0 ) );
*/

const int g_gameWaveEntities[MAX_WAVES][3]={
	{WAVE1_ASTEROID_NUM,WAVE1_BEETLE_NUM,WAVE1_WASP_NUM},
	{WAVE2_ASTEROID_NUM,WAVE2_BEETLE_NUM,WAVE2_WASP_NUM},
	{WAVE3_ASTEROID_NUM,WAVE3_BEETLE_NUM,WAVE3_WASP_NUM},
	{WAVE4_ASTEROID_NUM,WAVE4_BEETLE_NUM,WAVE4_WASP_NUM},
	{WAVE5_ASTEROID_NUM,WAVE5_BEETLE_NUM,WAVE5_WASP_NUM}
};
//REVIEW: should not exist. should be g_cameraPosition.....
const XboxController* playerController=nullptr;
const Vec2 cameraPosition=Vec2(100.f,50.f);



Game::Game( RenderContext* gameRenderer , InputSystem* inputSystem,bool isDevelop,Camera* inCamera)
	:m_theRenderer(gameRenderer)
	,m_theInput(inputSystem)
	,m_developMode(isDevelop)
	,m_camera(inCamera)
{
	m_rng=RandomNumberGenerator();
	playerController=&m_theInput->GetXboxController(0);
	yOffset=FIRE_PLACE_Y;//REVIEW: not clear
}

void Game::Startup()
{	
	m_playerShip=new PlayerShip(this,  Vec2(WORLD_CENTER_X,WORLD_CENTER_Y),playerController);
	m_playerShip->Startup();
	CreateWaves();
	GenerateWave(m_currentWave);
}
void Game::Shutdown()
{
	// player ship shutdown
	m_playerShip->Shutdown();
	delete m_playerShip;
	m_playerShip=nullptr;
	// asteroid shutdown
	for( int asteroidIndex=0; asteroidIndex<MAX_ASTEROIDS; asteroidIndex++ ) {
		if( m_asteroids[asteroidIndex]!=nullptr ) {
			m_asteroids[asteroidIndex]->Shutdown();
			delete m_asteroids[asteroidIndex];
			m_asteroids[asteroidIndex]=nullptr;
		}
	}
	//bullet shutdown
	for( int bulletIndex=0; bulletIndex<MAX_BULLETS; bulletIndex++ ) {
		if( m_bullets[bulletIndex]!=nullptr ) {
			m_bullets[bulletIndex]->Shutdown();
			delete m_bullets[bulletIndex];
			m_bullets[bulletIndex]=nullptr;
		}
	}
	


}

void Game::BeginFrame()
{
	m_theInput->BeginFrame();
}

void Game::RunFrame(float deltaTime)
{
	BeginFrame();
	ControlCollision();
	Update(deltaTime);
}

const void Game::Render()
{
	if( m_isAttractMode ) {

	}
	else{
	
		m_playerShip->Render();
		RenderBullets();
		RenderAsteroids();
		RenderBeetles();
		RenderWasps();
		RenderDebris();
	}
}

void Game::RenderBullets()
{
	for( int bulletIndex=0; bulletIndex<MAX_BULLETS; bulletIndex++ ) {
		if(m_bullets[bulletIndex]!=nullptr){
			m_bullets[bulletIndex]->Render();
			if( m_developMode ) {
				m_theRenderer->DrawLine( m_playerShip->m_position, m_bullets[bulletIndex]->m_position, LINE_THICK,  Rgba8( 50, 50, 50 ) );
			}
		}
	}
	
}

void Game::RenderAsteroids()
{
	for( int asteroidIndex=0; asteroidIndex<MAX_ASTEROIDS; asteroidIndex++ ) {
		if(m_asteroids[asteroidIndex]!=nullptr){
			m_asteroids[asteroidIndex]->Render();
			if( m_developMode ) {
				m_theRenderer->DrawLine( m_playerShip->m_position, m_asteroids[asteroidIndex]->m_position, LINE_THICK, Rgba8( 50, 50, 50 ) );
			}
		}
	}
	
}

void Game::SpawnPlayerShip()
{	//Review: use is dead to detect the death
	if(m_playerShip->m_isGarbage||m_playerShip->m_health==0){
		m_playerShip->Shutdown();
		
		
		m_playerShip->Reset(Vec2(WORLD_CENTER_X,WORLD_CENTER_Y));
	}
}

int Game::FindCurrentBeetlesIndex()
{
	for( int beetleIndex=0; beetleIndex<MAX_BEETLES; beetleIndex++ ) {
		if( m_beetles[beetleIndex]==nullptr ) {
			return beetleIndex;
		}
	}
	return MAX_BEETLES;
}

void Game::RenderWasps()
{
	for( int waspIndex=0; waspIndex<MAX_BEETLES; waspIndex++ ) {
		//REVIEW: can use is alive to check if exist. The code would be more concise
		if( m_wasps[waspIndex]!=nullptr ) {
			m_wasps[waspIndex]->Render();
			//REVIEW: Can move into render(); better place? Game is higher level? 
			if( m_developMode ) {
				m_theRenderer->DrawLine( m_playerShip->m_position, m_wasps[waspIndex]->m_position, LINE_THICK, Rgba8( 50, 50, 50 ) );
			}
		}
	}

}

void Game::SpawnWasp( PlayerShip& playerShip )
{
	int waspIndex=FindCurrentWaspssIndex();
	if( waspIndex<MAX_WASPS ) {
		float offset=m_rng.RollRandomFloatInRange(-30,30);
		Vec2 iniPosition= Vec2( WASP_SPAWN_POSITION_X, WASP_SPAWN_POSITION_Y+offset );

		Vec2 worldIniPosition=TransformPosition2D( Vec2( 0.f, 0.f ), NORMAL_SCALE, NORMAL_ROTATE_DEGREES, iniPosition );
		m_wasps[waspIndex]=new Wasp( this, worldIniPosition, &playerShip );
		m_wasps[waspIndex]->Startup();

	}

}

void Game::DestroyWasp()
{

}

void Game::UpdateWasps( float deltaTime )
{
	for( int waspIndex=0; waspIndex<MAX_WASPS; waspIndex++ ) {
		if( m_wasps[waspIndex]!=nullptr ) {
			m_wasps[waspIndex]->Update( deltaTime );
		}
	}

}

int Game::FindCurrentWaspssIndex()
{
	for( int waspIndex=0; waspIndex<MAX_WASPS; waspIndex++ ) {
		if( m_wasps[waspIndex]==nullptr ) {
			return waspIndex;
		}
	}
	return MAX_WASPS;
}

void Game::SpawnBullet(PlayerShip& playerShip)
	{
	int bulletIndex=FindCurrentBulletsIndex();
	if(bulletIndex<MAX_BULLETS){
		yOffset=-yOffset;
		
		Vec2 iniPosition= Vec2(FIRE_PLACE_X,yOffset);
		Vec2 playerShipForward=playerShip.GetForwardVector();
		float iniDegree=playerShipForward.GetAngleDegrees();
		Vec2 worldIniPosition=TransformPosition2D(iniPosition,NORMAL_SCALE,iniDegree,playerShip.m_position);   
		m_bullets[bulletIndex]=new Bullet(this,worldIniPosition,playerShip,iniDegree);
		m_bullets[bulletIndex]->Startup();
		
	}
	
	else{
		DisplayMessageBoxBullet();
	}
	
}

void Game::SpawnBulletWithAngleDegree( PlayerShip& playerShip, float degree )
{
	int bulletIndex=FindCurrentBulletsIndex();
	if( bulletIndex<MAX_BULLETS ) {
		Vec2 iniPosition= Vec2( FIRE_PLACE_X, FIRE_PLACE_Y );
		Vec2 playerShipForward=playerShip.GetForwardVector();
		float iniDegree=playerShipForward.GetAngleDegrees();
		iniDegree+=degree;
		Vec2 worldIniPosition=TransformPosition2D( iniPosition, NORMAL_SCALE, iniDegree, playerShip.m_position );
		m_bullets[bulletIndex]=new Bullet( this, worldIniPosition, playerShip, iniDegree );
		m_bullets[bulletIndex]->Startup();

	}

	else {
		DisplayMessageBoxBullet();
	}
}

void Game::SpawnBUlletsAll()
{
	float degree=0;
	for( int bulletIndex=0; bulletIndex<36; bulletIndex++ ) {
		SpawnBulletWithAngleDegree(*m_playerShip,degree);
		degree+=10;
	}
}

void Game::SpawnAsteroid()
{
	
	int asteroidIndex=FindCurrentAsteroidsIndex();

	if( asteroidIndex<MAX_ASTEROIDS ) {
		float iniX=(float)m_rng.RollRandomFloatInRange(2,198);
		float iniY=(float)m_rng.RollRandomFloatInRange(2,98);
		Vec2 iniPosition= Vec2( iniX,iniY );

		float iniDegree=(float)m_rng.RollRandomIntLessThan( 360 );
		Vec2 worldIniPosition=TransformPosition2D(  Vec2(0.f,0.f), NORMAL_SCALE, iniDegree, iniPosition );
		
		Vec2 iniVelocity= Vec2();
		iniVelocity.SetPolarDegrees(iniDegree,ASTEROID_SPEED);
		m_asteroids[asteroidIndex]=new Asteroid( this, worldIniPosition, iniVelocity );
		m_asteroids[asteroidIndex]->Startup();
		
	}

	else {
		DisplayMessageBoxAsteroid();
	}

}

void Game::DestroyPlayerShip()
{

}

void Game::DestroyBullet()
{

}

void Game::DestroyAsteroid()
{

}

int Game::DisplayMessageBoxAsteroid()
{

	int msgboxID = MessageBox(
		NULL,
		L"Cannot spawn Asteroid; already have max of 12 Asteroids\n\nWorld you like to continue running?\n (Yes=continue,No=quit)",
		L"Unnamed Application:: Warning",
		MB_ICONEXCLAMATION | MB_YESNO
	);
	if( msgboxID == 7 )
	{
		isAppQuit=true;
	}

	return msgboxID;
	
}

int Game::DisplayMessageBoxBullet()
{
	int msgboxID = MessageBox(
		NULL,
		L"Cannot spawn Bullet; already have max of 20 Bullets\n\nWorld you like to continue running?\n (Yes=continue,No=quit)",
		L"Unnamed Application:: Warning",
		MB_ICONEXCLAMATION | MB_YESNO
	);

	if( msgboxID ==  7 )
	{
		isAppQuit=true;
	}

	return msgboxID;
}

void Game::RotatePlayerShip( int direction )
{//REVIEW: input change the direction. better way?
	if(direction==1){
		m_playerShip->isRotate=1;
	}
	else if(direction ==-1){
		m_playerShip->isRotate=-1;
		
	}
	else{
		m_playerShip->isRotate=0;
	}
}

void Game::DeleteGarbageEntities()
{// REVIEW: difference between isGarbage and isDead? offScreen more reasonable?
	for (int i=0;i<MAX_BULLETS;i++){
		if(m_bullets[i]!=nullptr){
			if(m_bullets[i]->m_isGarbage){
 				delete(m_bullets[i]);
				m_bullets[i]=nullptr;
			}
		}
	}
	for( int i=0; i<MAX_ASTEROIDS; i++ ) {
		
		if(m_asteroids[i]!=nullptr){
			if( m_asteroids[i]->m_isGarbage ) {
				delete(m_asteroids[i]);
				m_asteroids[i]=nullptr;
			}
		}
	}

	for(int i=0;i<MAX_DEBRIS;i++){
		if(m_debris[i]!=nullptr){
			if(m_debris[i]->m_isGarbage){
				delete(m_debris[i]);
				m_debris[i]=nullptr;
			}
		}
	}
	// beetle
	for(int beetleIndex=0;beetleIndex<MAX_BEETLES;beetleIndex++){
		if( m_beetles[beetleIndex]!=nullptr ) {
			if( m_beetles[beetleIndex]->m_isGarbage ) {
				delete(m_beetles[beetleIndex]);
				m_beetles[beetleIndex]=nullptr;
			}
		}
	}
	//wasp
	for(int waspIndex=0; waspIndex<MAX_WASPS; waspIndex++) {
		if( m_wasps[waspIndex]!=nullptr ) {
			if( m_wasps[waspIndex]->m_isGarbage ) {
				delete(m_wasps[waspIndex]);
				m_wasps[waspIndex]=nullptr;
			}
		}
	}
	
}

void Game::SpeedUpPlayerShip(bool speedUp)
{//REVIEW: the game directly change the variable of playership. Better way to do better encapsulation
	if(speedUp)m_playerShip->isAccelerate=true;
	else m_playerShip->isAccelerate=false;
}

void Game::ControlCollision()
{//REVIEW: The codes are similar and redundant. Better way to do it? How Real engine do this?
	bool playershipResult=false;

	//player ship collide with asteroids
	for (int asteroirdIndex=0;asteroirdIndex<MAX_ASTEROIDS;asteroirdIndex++){
		if(m_asteroids[asteroirdIndex]!=nullptr){
			playershipResult=DoDiscsOverlap(m_playerShip->m_position,m_playerShip->m_physicsRadius,m_asteroids[asteroirdIndex]->m_position,m_asteroids[asteroirdIndex]->m_physicsRadius);
			if(playershipResult){
				//REVIEW: m_playerShip->TakeDamage(); better
				m_playerShip->m_health=0;
				m_currentLife--;
			
				m_asteroids[asteroirdIndex]->m_health=0;
			}
		}
	}
	//player ship collide with beetles
	for( int beetleIndex=0; beetleIndex<MAX_BEETLES; beetleIndex++ ) {
		if( m_beetles[beetleIndex]!=nullptr ) {
			playershipResult=DoDiscsOverlap( m_playerShip->m_position, m_playerShip->m_physicsRadius, m_beetles[beetleIndex]->m_position, m_beetles[beetleIndex]->m_physicsRadius );
			if( playershipResult&&m_playerShip->m_health>0 ) {
				m_playerShip->m_health=0;
				m_currentLife--;
				m_beetles[beetleIndex]->m_health=0;
			}
		}
	}
	//player ship collide with wasps
	for( int waspIndex=0; waspIndex<MAX_WASPS; waspIndex++ ) {
		if( m_wasps[waspIndex]!=nullptr ) {
			playershipResult=DoDiscsOverlap( m_playerShip->m_position, m_playerShip->m_physicsRadius, m_wasps[waspIndex]->m_position, m_wasps[waspIndex]->m_physicsRadius );
			if( playershipResult &&m_playerShip->m_health>0) {
				m_playerShip->m_health=0;
				m_currentLife--;
				m_wasps[waspIndex]->m_health=0;
			}
		}
	}
	//bullet and asteroid
	for (int asteroirdIndex=0;asteroirdIndex<MAX_ASTEROIDS;asteroirdIndex++){
		for(int bulletIndex=0;bulletIndex<MAX_BULLETS;bulletIndex++){
			bool AsteroidBulletResult=false; 
			if( m_asteroids[asteroirdIndex]!=nullptr&&m_bullets[bulletIndex]!=nullptr ) {
				AsteroidBulletResult=DoDiscsOverlap(m_bullets[bulletIndex]->m_position,m_bullets[bulletIndex]->m_physicsRadius,m_asteroids[asteroirdIndex]->m_position,m_asteroids[asteroirdIndex]->m_physicsRadius);
				if(AsteroidBulletResult){
					m_bullets[bulletIndex]->m_health=0;
					m_asteroids[asteroirdIndex]->m_health-=1;

				}
			}
		
		}
	}
	//bullet and beetles
	for( int beetleIndex=0; beetleIndex<MAX_BEETLES; beetleIndex++ ) {
		for( int bulletIndex=0; bulletIndex<MAX_BULLETS; bulletIndex++ ) {
			bool BeetleBulletResult=false;
			if( m_beetles[beetleIndex]!=nullptr&&m_bullets[bulletIndex]!=nullptr ) {
				BeetleBulletResult=DoDiscsOverlap( m_bullets[bulletIndex]->m_position, m_bullets[bulletIndex]->m_physicsRadius, m_beetles[beetleIndex]->m_position, m_beetles[beetleIndex]->m_physicsRadius );
				if( BeetleBulletResult ) {
					m_bullets[bulletIndex]->m_health=0;
					m_beetles[beetleIndex]->m_health-=1;

				}
			}

		}
	}
	//bullet and wasps
	for( int waspIndex=0; waspIndex<MAX_WASPS; waspIndex++ ) {
		for( int bulletIndex=0; bulletIndex<MAX_BULLETS; bulletIndex++ ) {
			bool WaspBulletResult=false;
			if( m_wasps[waspIndex]!=nullptr&&m_bullets[bulletIndex]!=nullptr ) {
				WaspBulletResult=DoDiscsOverlap( m_bullets[bulletIndex]->m_position, m_bullets[bulletIndex]->m_physicsRadius, m_wasps[waspIndex]->m_position, m_wasps[waspIndex]->m_physicsRadius );
				if( WaspBulletResult ) {
					m_bullets[bulletIndex]->m_health=0;
					m_wasps[waspIndex]->m_health-=1;

				}
			}

		}
	}

}





void Game::RenderUI()
{
	Vertex_PCU UI_shape[SHIP_VERTEX_NUM]={
		//triangle A
		Vertex_PCU( Vec3( -2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 0,2,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		
		Vertex_PCU( Vec3( -2, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( -2,-1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		
		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 1, 0, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		
		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -2, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) )

	};
	Vertex_PCU first_shape[SHIP_VERTEX_NUM]={
		//triangle A
		Vertex_PCU( Vec3( -2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 0,2,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),

		Vertex_PCU( Vec3( -2, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( -2,-1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),

		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),

		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 1, 0, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),

		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -2, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) )
	};
	Vertex_PCU second_shape[SHIP_VERTEX_NUM]={
		//triangle A
		Vertex_PCU( Vec3( -2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 0,2,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),

		Vertex_PCU( Vec3( -2, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( -2,-1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),

		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),

		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 1, 0, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),

		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -2, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) )
	};
	Vertex_PCU third_shape[SHIP_VERTEX_NUM]={
		//triangle A
		Vertex_PCU( Vec3( -2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 0,2,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),

		Vertex_PCU( Vec3( -2, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( -2,-1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),

		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),

		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 1, 0, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),

		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -2, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) )
	};
	Vec2 firstShip=Vec2(205,195);
	Vec2 secondShip=Vec2(210,195);
	Vec2 thirdShip=Vec2(215,195);
	Rgba8 circleColor=Rgba8(255,255,255);
	for(int vertexIndex=0;vertexIndex<SHIP_VERTEX_NUM;vertexIndex++){
		Vec3 WorldPos1=TransformPosition3DXY( UI_shape[vertexIndex].m_pos, 0.5, NORMAL_ROTATE_DEGREES,firstShip );
		first_shape[vertexIndex].m_pos=WorldPos1;
		Vec3 WorldPos2=TransformPosition3DXY( UI_shape[vertexIndex].m_pos, 0.5, NORMAL_ROTATE_DEGREES, secondShip );
		second_shape[vertexIndex].m_pos=WorldPos2;
		Vec3 WorldPos3=TransformPosition3DXY( UI_shape[vertexIndex].m_pos,0.5, NORMAL_ROTATE_DEGREES, thirdShip );
		third_shape[vertexIndex].m_pos=WorldPos3;
	}
		if( m_isAttractMode ) {
			m_theRenderer->DrawCircle( Vec3( 300, 150, 0 ), 30.f, 2.f, circleColor );
			m_theRenderer->DrawLine( Vec2( 300, 110 ), Vec2( 300, 190 ), 2.f, circleColor );
			m_theRenderer->DrawLine( Vec2( 270, 150 ), Vec2( 330, 150 ), 2.f, circleColor );
		}
		else{
			//REVIEW: use 2d array for shapes and use m_currentLife as index
		if(m_currentLife==4){
			m_theRenderer->DrawVertexArray(SHIP_VERTEX_NUM,first_shape);
			m_theRenderer->DrawVertexArray(SHIP_VERTEX_NUM,second_shape);
			m_theRenderer->DrawVertexArray(SHIP_VERTEX_NUM,third_shape);
		}
		else if(m_currentLife==3){
			m_theRenderer->DrawVertexArray( SHIP_VERTEX_NUM, first_shape );
			m_theRenderer->DrawVertexArray( SHIP_VERTEX_NUM, second_shape );
		}
		else if(m_currentLife==2){
			m_theRenderer->DrawVertexArray( SHIP_VERTEX_NUM, first_shape );
		}
	}
	
}

void Game::Reset()
{	
	m_BulletsAllCoolDown=MAX_BULLET_ALL_COOLDOWN;
	m_currentLife=4;
	m_currentWave=0;
	m_screenShakeIntensity=0;
	yOffset=FIRE_PLACE_Y;
	for( int bulletIndex=0; bulletIndex<MAX_BULLETS; bulletIndex++ ) {
		if( m_bullets[bulletIndex]!=nullptr ) {
			delete m_bullets[bulletIndex];
		}
		m_bullets[bulletIndex]=nullptr;
	}
	for( int asteroidIndex=0; asteroidIndex<MAX_ASTEROIDS; asteroidIndex++ ) {
		if( m_asteroids[asteroidIndex]!=nullptr ) {
			delete m_asteroids[asteroidIndex];
		}
		m_asteroids[asteroidIndex]=nullptr;
	}
	for( int beetleIndex=0; beetleIndex<MAX_BEETLES; beetleIndex++ ) {
		if( m_beetles[beetleIndex]!=nullptr ) {
			delete m_beetles[beetleIndex];
		}
		m_beetles[beetleIndex]=nullptr;
	}
	for( int waspIndex=0; waspIndex<MAX_WASPS; waspIndex++ ) {
		if( m_wasps[waspIndex]!=nullptr ) {
			delete m_wasps[waspIndex];
		}
		m_wasps[waspIndex]=nullptr;
	}
	for(int debrisIndex=0;debrisIndex<MAX_DEBRIS;debrisIndex++){
		if(m_debris[debrisIndex]!=nullptr){
			delete m_debris[debrisIndex];
		}
		m_debris[debrisIndex]=nullptr;
	}
	Startup();
	
}

void Game::UpdateCoolDown(float deltaTime)
{
	m_BulletsAllCoolDown+=deltaTime;
	if(m_BulletsAllCoolDown>MAX_BULLET_ALL_COOLDOWN){
		m_enableSpawnBulletsAll=true;
		m_BulletsAllCoolDown=0.f;
	}
	


}

void Game::CreateWaves()
{
	for(int waveIndex=0;waveIndex<MAX_WAVES;waveIndex++){
		m_waves[waveIndex]=new Wave(this,g_gameWaveEntities[waveIndex][0],g_gameWaveEntities[waveIndex][1],g_gameWaveEntities[waveIndex][2]);
	}
}

void Game::GenerateWave( int waveIndex )
{
  	m_waves[waveIndex]->CreateEntities();
}

bool Game::CheckWaveComplete()
{
	for( int AsteroidIndex=0; AsteroidIndex<MAX_ASTEROIDS; AsteroidIndex++ ) {
		if( m_asteroids[AsteroidIndex]!=nullptr ) {
			return false;
		}
	}
	for( int beetleIndex=0; beetleIndex<MAX_BEETLES; beetleIndex++ ) {
		if( m_beetles[beetleIndex]!=nullptr ) {
			return false;
		}
	}
	for( int waspIndex=0; waspIndex<MAX_WASPS; waspIndex++ ) {
		if( m_wasps[waspIndex]!=nullptr ) {
			return false;
		}
	}
	return true;
}

int Game::FindCurrentBulletsIndex()
{
	for( int bulletIndex=0; bulletIndex<MAX_BULLETS; bulletIndex++ ) {
		if(m_bullets[bulletIndex]==nullptr){
			return bulletIndex;
		}
	}
	return MAX_BULLETS;
}

int Game::FindCurrentAsteroidsIndex()
{
	for( int AsteroidIndex=0; AsteroidIndex<MAX_ASTEROIDS; AsteroidIndex++ ) {
		if( m_asteroids[AsteroidIndex]==nullptr ) {
			return AsteroidIndex;
		}
	}
	return MAX_ASTEROIDS;
}

void Game::Fire()
{
	
}

void Game::RespawnFromXbox()
{
	if(playerController->m_buttonStates[XBOX_BUTTON_ID_START].IsPressed()){
		if(m_currentLife>0){
			SpawnPlayerShip();
		}
	}
}

void Game::RenderBeetles()
{
	for( int beetleIndex=0; beetleIndex<MAX_BEETLES; beetleIndex++ ) {
		if( m_beetles[beetleIndex]!=nullptr ) {
			m_beetles[beetleIndex]->Render();
			if( m_developMode ) {
				m_theRenderer->DrawLine( m_playerShip->m_position, m_beetles[beetleIndex]->m_position, LINE_THICK, Rgba8( 50, 50, 50 ) );
			}
		}
	}
}

void Game::SpawnBeetle( PlayerShip& playerShip )
{
	int beetleIndex=FindCurrentBeetlesIndex();
	if( beetleIndex<MAX_BEETLES ) {
		float offset=m_rng.RollRandomFloatInRange(-40,40);
		Vec2 iniPosition= Vec2( BEETLE_SPAWN_POSITION_X, BEETLE_SPAWN_POSITION_Y+offset );
		Vec2 worldIniPosition=TransformPosition2D( Vec2(0.f,0.f), NORMAL_SCALE, NORMAL_ROTATE_DEGREES, iniPosition );
		m_beetles[beetleIndex]=new Beetle( this, worldIniPosition, &playerShip );
		m_beetles[beetleIndex]->Startup();

	}
}

void Game::DestroyBeetle()
{

}

void Game::UpdateBeetles( float deltaTime )
{
	for( int beetleIndex=0; beetleIndex<MAX_BEETLES; beetleIndex++ ) {
		if( m_beetles[beetleIndex]!=nullptr ) {
			m_beetles[beetleIndex]->Update( deltaTime );
		}
	}
}

void Game::CameraShake(  )
{
	m_screenShakeIntensity=MAX_CAMERA_SHAKE_INTENSITY;
}



bool Game::HandlePlayerInputPressed( unsigned char keyCode )
{
	if(m_isAttractMode){
		switch(keyCode )
		{
		case VK_SPACE:
			m_isAttractMode=false;
			Reset();
			break;
		case (unsigned char)'N':
			m_isAttractMode=false;
			Reset();
			break;	
		default:
			break;
		}
	}
	switch (keyCode)
	{
		case VK_LEFT:
			//rotate anticlockwise
			RotatePlayerShip(1);
			break;
		case VK_RIGHT:
			RotatePlayerShip(-1);
			break;
		case VK_UP:
			SpeedUpPlayerShip(true);
			break;
		case VK_SPACE:
			if(m_enableSpawnBullet){
				
				SpawnBullet(*m_playerShip);
				m_enableSpawnBullet=false;
			}
			break;
		case (unsigned char)'N':
			if(m_currentLife>0){
				SpawnPlayerShip();
			}
			break;
		case (unsigned char)'O':
			if(m_enableSpawnAsteroid){
				SpawnAsteroid();
				m_enableSpawnAsteroid=false;
			}
			break;
		default:
			break;
	}
	return false;
}

bool Game::HandlePlayerInputReleased( unsigned char keyCode )
{
	switch(keyCode){
		case VK_LEFT:
			//rotate anticlockwise
			RotatePlayerShip( 0 );
			break;
		case VK_RIGHT:
			RotatePlayerShip( 0 );
			break;
		case VK_UP:
			SpeedUpPlayerShip(false);
			break;
		case VK_SPACE:
			m_enableSpawnBullet=true;
			break;
		case (unsigned char)'O':
			m_enableSpawnAsteroid=true;
		default:
			break;
	}
	return true;
}

void Game::Update(float deltaTime)
{
	if(m_isAttractMode){
		if(playerController->m_buttonStates[XBOX_BUTTON_ID_START].IsPressed()){
			m_isAttractMode=false;
			Reset();
		}
	}
	else{
		UpdateShip(deltaTime);
		UpdateBullets(deltaTime);
		UpdateAsteroids(deltaTime);
		UpdateBeetles(deltaTime);
		UpdateWasps(deltaTime);
		UpdateDebris(deltaTime);
		UpdateCamera(deltaTime);
		UpdateCoolDown(deltaTime);
		bool waveResult=CheckWaveComplete();
		if(waveResult){
			m_currentWave++;
			if(m_currentWave<5){
			GenerateWave(m_currentWave);
			}
			else{
				m_isAttractMode=true;
			}
		}
		DeleteGarbageEntities();
		if(m_playerShip->m_health==0){
			m_enableSpawnBullet=false;
		}
	}
	
}

void Game::UpdateShip(float deltaTime)
{
	m_playerShip->Update(deltaTime);
	ShootFromXbox();
	RespawnFromXbox();
	int debrisIndex;
	if(m_currentLife==0){
		for(debrisIndex=0;debrisIndex<MAX_DEBRIS;debrisIndex++){
			if(m_debris[debrisIndex]!=nullptr){
				break;
			}
		}
		if(debrisIndex==MAX_DEBRIS){
			m_currentLife--;
		}
	}
	if(m_currentLife<0){
		m_isAttractMode=true;
	}
}

void Game::UpdateBullets(float deltaTime)
{

	for(int bulletIndex=0;bulletIndex<MAX_BULLETS;bulletIndex++){
		if(m_bullets[bulletIndex]!=nullptr){
		m_bullets[bulletIndex]->Update(deltaTime);
		}
	}
}

void Game::UpdateAsteroids(float deltaTime)
{
	
	for( int asteroidIndex=0; asteroidIndex<MAX_ASTEROIDS; asteroidIndex++ ) {
		if(m_asteroids[asteroidIndex]!=nullptr){
			m_asteroids[asteroidIndex]->Update( deltaTime );
		}
	}

}
//
//


// Can use polymorphism to reduce the redundant
void Game::SpawnBulletDebris(Bullet& deadBody)
{
	float degree=0;
	float lastDegree=0;
	for(int debrisIndex=0;debrisIndex<deadBody.m_debrisNum;debrisIndex++  ){
		int debrisIndexInGame=FindCurrentDebrisIndex();
		degree=lastDegree+360/deadBody.m_debrisNum;
		if( debrisIndexInGame<MAX_DEBRIS ) {
		
			Vec2 iniPosition= deadBody.m_position;

			float iniDegree=(float)m_rng.RollRandomFloatInRange(lastDegree,degree) ;
			Vec2 worldIniPosition=TransformPosition2D( Vec2( 0.f, 0.f ), NORMAL_SCALE, iniDegree, iniPosition );

			Vec2 iniVelocity= Vec2();
			iniVelocity.SetPolarDegrees( iniDegree, DEBRIS_SPEED );
			m_debris[debrisIndexInGame]=new Debris( this, worldIniPosition, iniVelocity,deadBody.m_color);
			m_debris[debrisIndexInGame]->Startup();

		}

		else {
		}
	}
}
void Game::SpawnPlayerShipDebris( PlayerShip& deadBody )
{
	float degree=0;
	float lastDegree=0;
	for( int debrisIndex=0; debrisIndex<deadBody.m_debrisNum; debrisIndex++ ) {
		int debrisIndexInGame=FindCurrentDebrisIndex();
		degree=lastDegree+360/deadBody.m_debrisNum;
		if( debrisIndexInGame<MAX_DEBRIS ) {

			Vec2 iniPosition= deadBody.m_position;

			float iniDegree=(float)m_rng.RollRandomFloatInRange( lastDegree, degree );
			Vec2 worldIniPosition=TransformPosition2D( Vec2( 0.f, 0.f ), NORMAL_SCALE, iniDegree, iniPosition );

			Vec2 iniVelocity= Vec2();
			iniVelocity.SetPolarDegrees( iniDegree, DEBRIS_SPEED );
			m_debris[debrisIndexInGame]=new Debris( this, worldIniPosition, iniVelocity, deadBody.m_color );
			m_debris[debrisIndexInGame]->Startup();

		}
		else {
		}
		lastDegree=degree;
	}
}
void Game::SpawnAsteroidDebris( Asteroid& deadBody )
{
	float degree=0;
	float lastDegree=0;
	for( int debrisIndex=0; debrisIndex<deadBody.m_debrisNum; debrisIndex++ ) {
		int debrisIndexInGame=FindCurrentDebrisIndex();
		degree=lastDegree+360/deadBody.m_debrisNum;
		if( debrisIndexInGame<MAX_DEBRIS ) {

			Vec2 iniPosition= deadBody.m_position;

			float iniDegree=(float)m_rng.RollRandomFloatInRange( lastDegree, degree );
			Vec2 worldIniPosition=TransformPosition2D( Vec2( 0.f, 0.f ), NORMAL_SCALE, iniDegree, iniPosition );

			Vec2 iniVelocity= Vec2();
			iniVelocity.SetPolarDegrees( iniDegree, DEBRIS_SPEED );
			m_debris[debrisIndexInGame]=new Debris( this, worldIniPosition, iniVelocity, deadBody.m_color );
			m_debris[debrisIndexInGame]->Startup();

		}

		else {
		}
		lastDegree=degree;
	}
}
void Game::SpawnBeetleDebris( Beetle& deadBody )
{
	float degree=0;
	float lastDegree=0;
	for( int debrisIndex=0; debrisIndex<deadBody.m_debrisNum; debrisIndex++ ) {
		int debrisIndexInGame=FindCurrentDebrisIndex();
		degree=lastDegree+360/deadBody.m_debrisNum;
		if( debrisIndexInGame<MAX_DEBRIS ) {

			Vec2 iniPosition= deadBody.m_position;

			float iniDegree=(float)m_rng.RollRandomFloatInRange( lastDegree, degree );
			Vec2 worldIniPosition=TransformPosition2D( Vec2( 0.f, 0.f ), NORMAL_SCALE, iniDegree, iniPosition );

			Vec2 iniVelocity= Vec2();
			iniVelocity.SetPolarDegrees( iniDegree, DEBRIS_SPEED );
			m_debris[debrisIndexInGame]=new Debris( this, worldIniPosition, iniVelocity, deadBody.m_color );
			m_debris[debrisIndexInGame]->Startup();

		}

		else {
		}
		lastDegree=degree;
	}
}
void Game::SpawnWaspDebris( Wasp& deadBody )
{
	float degree=0;
	float lastDegree=0;
	for( int debrisIndex=0; debrisIndex<deadBody.m_debrisNum; debrisIndex++ ) {
		int debrisIndexInGame=FindCurrentDebrisIndex();
		degree=lastDegree+360/deadBody.m_debrisNum;
		if( debrisIndexInGame<MAX_DEBRIS ) {

			Vec2 iniPosition= deadBody.m_position;

			float iniDegree=(float)m_rng.RollRandomFloatInRange( lastDegree, degree );
			Vec2 worldIniPosition=TransformPosition2D( Vec2( 0.f, 0.f ), NORMAL_SCALE, iniDegree, iniPosition );

			Vec2 iniVelocity= Vec2();
			iniVelocity.SetPolarDegrees( iniDegree, DEBRIS_SPEED );
			m_debris[debrisIndexInGame]=new Debris( this, worldIniPosition, iniVelocity, deadBody.m_color );
			m_debris[debrisIndexInGame]->Startup();

		}

		else {
		}
		lastDegree=degree;
	}
}

void Game::DestroyDebris()
{

}

void Game::RenderDebris()
{
	for( int DebrisIndex=0; DebrisIndex<MAX_DEBRIS; DebrisIndex++ ) {
		if( m_debris[DebrisIndex]!=nullptr ) {
			m_debris[DebrisIndex]->Render();
			if( m_developMode ) {
				m_theRenderer->DrawLine( m_playerShip->m_position, m_debris[DebrisIndex]->m_position, LINE_THICK, Rgba8( 50, 50, 50 ) );
			}
		}
	}
}

int Game::FindCurrentDebrisIndex()
{

	for( int debrisIndex=0; debrisIndex<MAX_DEBRIS; debrisIndex++ ) {
		if( m_debris[debrisIndex]==nullptr ) {
			return debrisIndex;
		}
	}
	return MAX_DEBRIS;
}

void Game::UpdateDebris( float deltaTime )
{
	for( int debrisIndex=0; debrisIndex<MAX_DEBRIS; debrisIndex++ ) {
		if( m_debris[debrisIndex]!=nullptr ) {
			m_debris[debrisIndex]->Update( deltaTime );
		}
	}
}

void Game::UpdateCamera( float deltaTime )
{
	m_screenShakeIntensity-=m_screenShakeDecreaseRate*deltaTime;
	if(m_screenShakeIntensity<0){
		m_screenShakeIntensity=0;
	}
	float maxShakeDist=m_screenShakeIntensity*MAX_CAMERA_SHAKE_VALUE;
	float cameraShakeX=m_rng.RollRandomFloatInRange(-maxShakeDist,maxShakeDist);
	float cameraShakeY=m_rng.RollRandomFloatInRange(-maxShakeDist,maxShakeDist);
	Vec2 temCameraPosition=Vec2(cameraPosition.x+cameraShakeX,cameraPosition.y+cameraShakeY);
	m_camera->SetPosition(temCameraPosition);


}

void Game:: ShootFromXbox(){
	if(playerController->m_buttonStates[XBOX_BUTTON_ID_A].WasJustPressed()){
		if(m_enableSpawnBullet){
			SpawnBullet( *m_playerShip );
			m_enableSpawnBullet=false;
		}
	}
	if(playerController->m_buttonStates[XBOX_BUTTON_ID_A].WasJustReleased()){
		m_enableSpawnBullet=true;
	}
	//all direction
	if( playerController->m_buttonStates[XBOX_BUTTON_ID_B].WasJustPressed() ) {
		if( m_enableSpawnBulletsAll ) {
			SpawnBUlletsAll( );
			m_BulletsAllCoolDown=0;
			m_enableSpawnBulletsAll=false;
		}
	}
	
}
