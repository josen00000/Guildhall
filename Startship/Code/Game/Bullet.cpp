#include "Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Bullet::Bullet( Game* iniGame, Vec2& iniPos,PlayerShip& iniPlayerShip,float forwardDegree )
{
	m_game=iniGame;
	m_position=iniPos;
	m_PlayerShip=&iniPlayerShip;
	m_forwardDegree=forwardDegree;
	m_health=1;
	m_physicsRadius=BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius=BULLET_COSMETIC_RADIUS;
	m_vertexNum=BULLET_VERTEX_NUM;
	m_debrisNum=BULLET_DEBRIS_NUM;
	m_color=Rgba8(255,0,0 );
}

void Bullet::Startup()
{
	CreateBullet();
	m_velocity=GetBulletVelocity();

}

void Bullet::Shutdown()
{
}

void Bullet::CreateBullet()
{
	//triangle A
	m_shape[0]=Vertex_PCU( Vec3( -2.f, 0.f, 0.f ), Rgba8(255,0,0,0 ), Vec2( 0, 0 ) );
	m_shape[1]=Vertex_PCU( Vec3( 0.f, .5f, 0.f ), Rgba8( 100, 100, 100 ), Vec2( 0, 0 ) );
	m_shape[2]=Vertex_PCU( Vec3( 0.f, -.5f, 0.f ), Rgba8( 100, 100, 100 ), Vec2( 0, 0 ) );
	//triangle B
	m_shape[3]=Vertex_PCU( Vec3( 0.f, .5f, 0.f ), Rgba8( 100, 100, 100 ), Vec2( 0, 0 ) );
	m_shape[4]=Vertex_PCU( Vec3( 0.f, -.5f, 0.f ), Rgba8( 100, 100, 100 ), Vec2( 0, 0 ) );
	m_shape[5]=Vertex_PCU( Vec3( 0.5f, 0.f, 0.f ), Rgba8(255, 255,  0 ), Vec2( 0, 0 ) );
}

void Bullet::GenerateDebris()
{
	m_game->SpawnBulletDebris(*this);
	ableDebri=false;
}

void Bullet::Update(float deltaTime){
	if(m_health==0&&ableDebri){
		GenerateDebris();
	}
	Entity::Update(deltaTime);
}

const void Bullet::Render()
{
	if( m_game->m_developMode ) {
		Entity::Render();
	}
	
	static Vertex_PCU worldShape[]={
		//triangle A
		Vertex_PCU( Vec3( -2.f, 0.f, 0.f ), Rgba8( 255,0,0,0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0.f, .5f, 0.f ), Rgba8( 255, 0,  0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0.f, .5f, 0.f ), Rgba8( 255, 0,  0 ), Vec2( 0, 0 ) ),
		//triangle B							  
		Vertex_PCU( Vec3( 0.f, .5f, 0.f ), Rgba8( 255, 0,  0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0.f, .5f, 0.f ), Rgba8( 255, 0,  0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0.5f, 0.f, 0.f ), Rgba8( 255, 255,  0), Vec2( 0, 0 ) )
	};

	for( int vertexIndex=0; vertexIndex<m_vertexNum; vertexIndex++ ) {
		Vec3 worldPosition=TransformPosition3DXY( m_shape[vertexIndex].m_pos, NORMAL_SCALE, m_forwardDegree, m_position );
		worldShape[vertexIndex].m_pos=worldPosition;
	}

 	m_game->m_theRenderer->DrawVertexArray( m_vertexNum, worldShape );
	
}

void Bullet::Die()
{
	m_isDead=true;
}



Vec2 Bullet::GetBulletVelocity()
{
	
	Vec2 direction=Vec2(0,0);
	direction.SetPolarDegrees(m_forwardDegree,BULLET_SPEED);
	return direction;

}
