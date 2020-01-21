#include "Debris.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Debris::Debris( Game* iniGame, Vec2& iniPos,Vec2 iniVelocity, Rgba8& iniColor )
{
	m_game=iniGame;
	m_position= iniPos;
	m_velocity=iniVelocity;
	m_physicsRadius=.3f;
	m_cosmeticRadius=.8f;
	m_vertexNum=ASTEROID_VERTEX_NUM;
	m_rotateDegree=0;
	m_color=iniColor;
	m_angularVelocity=(float)m_game->m_rng.RollRandomFloatInRange( -200, 200 );
	m_color=iniColor;
	m_color.a=(unsigned char)127.f;
	m_health=1;
}

void Debris::Startup()
{
	
	CreateDebri();

}

void Debris::Shutdown()
{
}

void Debris::Update( float deltaTime )
{
	UpdateRotation(deltaTime);
	FadeoutUpdate(deltaTime);
	if(m_color.a<=0){
		m_health=0;
	}
	Entity::Update(deltaTime);

}

const void Debris::Render()
{
	static Vertex_PCU worldShape[DEBRIS_VERTEX_NUM];
	for (int debrisIndex=0;debrisIndex<ASTEROID_VERTEX_NUM;debrisIndex++){
		worldShape[debrisIndex]= Vertex_PCU(m_shape[debrisIndex].m_pos,m_color, Vec2( 0, 0 ));
	}
	for( int debrisIndex=0; debrisIndex<m_vertexNum; debrisIndex++ ) {
		Vec3 worldPosition=TransformPosition3DXY( m_shape[debrisIndex].m_pos, NORMAL_SCALE, m_rotateDegree, m_position );
		worldShape[debrisIndex].m_pos=worldPosition;
	}
	m_game->m_theRenderer->DrawVertexArray(m_vertexNum,worldShape);
//	Entity::Render();
}






void Debris::CreateDebri()
{
	float lastLength=0;
	float lastDegree=0;
	float nextLength=0;
	float nextDegree=0;
	for(int debrisIndex=0;debrisIndex<DEBRIS_VERTEX_NUM;debrisIndex++){
		if(debrisIndex%3==0){
			m_shape[debrisIndex]=Vertex_PCU( Vec3( 0.f, 0.f, 0.f ), Rgba8( 0, 0, 0 ), Vec2( 0, 0 ) );
		}
		else if(debrisIndex%3==1){
			if(lastLength==0){
				lastLength=m_game->m_rng.RollRandomFloatInRange(m_physicsRadius,m_cosmeticRadius);
			}
			Vec2 position= Vec2();
			position.SetPolarDegrees(lastDegree,lastLength);
			m_shape[debrisIndex]=Vertex_PCU( Vec3( position, 0.f ), Rgba8( 0, 0, 0 ), Vec2( 0, 0 ) );
		}
		else if(debrisIndex%3==2){
			nextDegree=lastDegree+360.f/16.f;
			nextLength=m_game->m_rng.RollRandomFloatInRange(m_physicsRadius,m_cosmeticRadius);
			Vec2 position1= Vec2();
			position1.SetPolarDegrees(nextDegree,nextLength);
			m_shape[debrisIndex]=Vertex_PCU( Vec3(position1, 0.f ), Rgba8( 0, 0, 0 ), Vec2( 0, 0 ) );
			//update degrees and length
			lastDegree+=360.f/16.f;
			lastLength=nextLength;
		}
	}
} 

void Debris::UpdateRotation(float deltaTime)
{
	m_rotateDegree+=m_angularVelocity*deltaTime;

}

void Debris::FadeoutUpdate(float deltaTime)
{
	float tem=m_color.a;
		tem-=m_fadeRate*deltaTime;
	if(tem<0){
		tem=0;
	}
	m_color.a=(unsigned char)tem;
}
