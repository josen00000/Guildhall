#include <stdio.h>
#include <string.h>
#include "PlayerShip.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Renderer/RenderContext.hpp"

PlayerShip::PlayerShip( Game* iniGame, Vec2 initialPos, const XboxController* iniXboxController )
{
	m_game=iniGame;
	m_physicsRadius=PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius=PLAYER_SHIP_COSMETIC_RADIUS;
	m_health=1;
	m_velocity= Vec2( 0.f, 0.f );
	m_position= Vec2(initialPos);
	m_vertexNum=SHIP_VERTEX_NUM;
	m_scale=NORMAL_SCALE;
	m_rotateDegree=NORMAL_ROTATE_DEGREES;
	m_xboxController= iniXboxController ;
	m_debrisNum=PLAYER_SHIP_DEBRIS_NUM;
	m_color=Rgba8( 132, 156, 169 );
	
}

void PlayerShip::Startup()
{
	CreateShip();
}

void PlayerShip::Shutdown()
{

}

void PlayerShip::Update(float deltaTime)
{
	if(m_isDead){
		return;
	}
	UpdateVibration(deltaTime);
	if(m_health<=0&&!m_isDead){
		Die();
		
		
		return;
	}
	CheckCollision();
	UpdateFromXBox(deltaTime);
	UpdateSTRA(deltaTime,isRotate);
	Entity::Update(deltaTime);
	
}

const void PlayerShip::Render()
{
	if( m_game->m_developMode ) {
		Entity::Render();
	}
	if(m_health==0){
		
		return;
	};
	static Vertex_PCU worldShape[]={
		//vertex A
		Vertex_PCU( Vec3( -2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 0,2,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) ),
		//vertex B
		Vertex_PCU( Vec3( -2, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( -2,-1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		//vertex C
		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		//vertex D
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 1, 0, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		//vertex E
		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -2, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 2, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) )

	};
	
	for( int vertexIndex=0;vertexIndex<m_vertexNum; vertexIndex++  ) {
		Vec3 worldPosition=TransformPosition3DXY( m_shape[vertexIndex].m_pos, m_scale, m_rotateDegree,m_position );
		worldShape[vertexIndex].m_pos=worldPosition;
	}
	m_game->m_theRenderer->DrawVertexArray(m_vertexNum,worldShape);
}

void PlayerShip::Die()
{
	
	if( ableDebri ) {
		GenerateDebris();
		//m_xboxController->GetRightTrigger();
		m_vibrationIntense=1;
		m_game->CameraShake();
	}

}



void PlayerShip::CreateShip()
{
	
	//triangle A
	m_shape[0]=Vertex_PCU( Vec3( -2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) );
	m_shape[1]=Vertex_PCU( Vec3( 0,2,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) );
	m_shape[2]=Vertex_PCU( Vec3( 2,1,0 ),Rgba8( 132, 156, 169 ),Vec2( 0,0 ) );
	//triangle B
	m_shape[3]=Vertex_PCU( Vec3( -2, 1, 0 ), Rgba8(132, 156, 169 ), Vec2( 0, 0 ) );
	m_shape[4]=Vertex_PCU( Vec3( -2,-1, 0), Rgba8( 132, 156, 169), Vec2( 0, 0 ) );
	m_shape[5]=Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169), Vec2( 0, 0 ) );
	//triangle C
	m_shape[6]=Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8(132, 156, 169 ), Vec2( 0, 0 ) );
	m_shape[7]=Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) );
	m_shape[8]=Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169), Vec2( 0, 0 ) );
	//triangle D
	m_shape[9]=Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 156, 169), Vec2( 0, 0 ) );
	m_shape[10]=Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8(132, 156, 169 ), Vec2( 0, 0 ) );
	m_shape[11]=Vertex_PCU( Vec3( 1, 0, 0 ), Rgba8( 132, 156, 169 ), Vec2( 0, 0 ) );
	//triangle E
	m_shape[12]=Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8(132, 156, 169 ), Vec2( 0, 0 ) );
	m_shape[13]=Vertex_PCU( Vec3( 0, -2, 0 ), Rgba8( 132, 156, 169), Vec2( 0, 0 ) );
	m_shape[14]=Vertex_PCU( Vec3( 2, -1, 0 ), Rgba8( 132, 156, 169), Vec2( 0, 0 ) );

	
	
}

void PlayerShip::CheckCollision()
{
	if(Entity::IsCollisionWithScreen()){
		BounceAway();
	}
}



void PlayerShip::BounceAway( )
{
	m_velocity=-m_velocity;
	if( m_position.x<m_game->s_leftdown.x+m_physicsRadius ) {
		m_velocity.y=-m_velocity.y;
	}
	else if( m_position.x>m_game->s_rightup.x-m_physicsRadius ) {
		m_velocity.y=-m_velocity.y;
	}
	else if( m_position.y>m_game->s_rightup.y-m_physicsRadius ) {
		m_velocity.x=-m_velocity.x;

	}
	else if( m_position.y<m_game->s_leftdown.y+m_physicsRadius ) {
		m_velocity.x=-m_velocity.x;
	}

}

void PlayerShip::UpdateSTRA( float deltaTime,int rotDirection )
{
	if(isAccelerate){
		m_velocity+=m_accelerate*this->GetForwardVector()*deltaTime;		
	}
	

	m_rotateDegree+=rotDirection*PLAYER_SHIP_TURN_SPEED*deltaTime;

}

void PlayerShip::UpdateFromXBox(float deltaTime)
{

	//get left joy stick value
	if(m_xboxController->isConnected()){
		float inputAngleDegrees=m_xboxController->m_leftJoystick.GetAngleDegrees();
		float inputAngleMagnitude=m_xboxController->m_leftJoystick.GetMagnitude();
		if(deltaTime!=0){
		m_rotateDegree=inputAngleDegrees;
		}
		m_velocity+=m_accelerate*inputAngleMagnitude*this->GetForwardVector()*deltaTime;	
	}

}

void PlayerShip::GenerateDebris()
{
	m_game->SpawnPlayerShipDebris(*this);
	ableDebri=false;
}

void PlayerShip::Reset(Vec2 resetPos)
{
	m_health=1;
	m_velocity= Vec2( 0.f, 0.f );
	m_rotateDegree=NORMAL_ROTATE_DEGREES;
	m_position=resetPos;
	ableDebri=true;
	Startup();
}

void PlayerShip::UpdateVibration(float deltaTime)
{
	m_vibrationIntense-=m_vibrationDownRate*deltaTime;
	float tem=m_vibrationIntense;
	if(tem<=0){
		tem=0;
	}
	m_xboxController->setVibration(tem);
}



