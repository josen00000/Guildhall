#include <stdio.h>
#include "Wasp.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Engine/Renderer/RenderContext.hpp"
Wasp::Wasp( Game* iniGame, Vec2 initialPos, PlayerShip* inPlayerShip )
{
	m_game=iniGame;
	m_health=WASP_HEALTH;
	m_physicsRadius=WASP_PHYSICS_RADIUS;
	m_cosmeticRadius=WASP_COSMETIC_RADIUS;
	m_velocity= Vec2( 0.f, 0.f );
	m_position= Vec2(initialPos);
	m_vertexNum=WASP_VERTEX_NUM;
	m_scale=NORMAL_SCALE;
	m_rotateDegree=NORMAL_ROTATE_DEGREES;
	m_player=inPlayerShip;	
	m_debrisNum=WASP_DEBRIS_NUM;
	m_color=Rgba8( 132, 0, 0 );
	m_isEnemy=true;
}

void Wasp::Startup()
{
	CreateWasp();
}

void Wasp::Shutdown()
{

}

void Wasp::Update(float deltaTime)
{
	
	
	Vec2 direction=m_player->m_position-m_position;
	direction.Normalize();
	m_acceleration=direction.operator*(WASP_ACCELERATION);
	m_velocity+=m_acceleration*deltaTime;
	m_velocity.ClampLength(WASP_MAX_SPEED);
	m_rotateDegree=m_acceleration.GetAngleDegrees();
	Entity::Update(deltaTime);
	if(m_health==0&&ableDebri){
		GenerateDebris();
	}
	
	
}

const void Wasp::Render()
{
	
	if(m_health==0){return;}
	static Vertex_PCU worldShape[]={
		//vertex A
		Vertex_PCU( Vec3( -2,1,0 ),Rgba8( 132, 0, 0 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 0,2,0 ),Rgba8( 132, 0, 0 ),Vec2( 0,0 ) ),
		Vertex_PCU( Vec3( 2,1,0 ),Rgba8( 132, 0, 0 ),Vec2( 0,0 ) ),
		//vertex B
		Vertex_PCU( Vec3( -2, 1, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( -2,-1, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		//vertex C
		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		//vertex D
		Vertex_PCU( Vec3( 0, 1, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -1, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 1, 0, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		//vertex E
		Vertex_PCU( Vec3( -2, -1, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0, -2, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 2, -1, 0 ), Rgba8( 132, 0, 0 ), Vec2( 0, 0 ) )

	};
	
	for( int vertexIndex=0;vertexIndex<m_vertexNum; vertexIndex++  ) {
		Vec3 worldPosition=TransformPosition3DXY( m_shape[vertexIndex].m_pos, m_scale, m_rotateDegree,m_position );
		worldShape[vertexIndex].m_pos=worldPosition;
	}
	m_game->m_theRenderer->DrawVertexArray(m_vertexNum,worldShape);
	if( m_game->m_developMode ) {
		Entity::Render();
	}
}

void Wasp::Die()
{

}

void Wasp::CreateWasp()
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

void Wasp::GenerateDebris()
{
	m_game->SpawnWaspDebris(*this);
	ableDebri=false;
}







