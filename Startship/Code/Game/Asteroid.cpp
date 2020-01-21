#include"Asteroid.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Asteroid::Asteroid( Game* iniGame, Vec2& iniPos,Vec2 iniVelocity )
{
	m_game=iniGame;
	m_position= iniPos;
	m_velocity=iniVelocity;
	m_physicsRadius=1.6f;
	m_cosmeticRadius=2.0f;
	m_vertexNum=ASTEROID_VERTEX_NUM;
	m_rotateDegree=0;
	m_angularVelocity=(float)m_game->m_rng.RollRandomIntInRange( -200, 200 );
	m_health=3;
	m_debrisNum=ASTEROID_DEBRIS_NUM;
	m_color=Rgba8( 100, 100, 100 );
	
}

void Asteroid::Startup()
{
	
	CreateAsteroid();

}

void Asteroid::Shutdown()
{
}

void Asteroid::Update( float deltaTime )
{
	if(m_health==0&&ableDebri){
		GenerateDebris();
	}
	UpdateRotation(deltaTime);
	CheckHealth();
	Entity::Update(deltaTime);

}

const void Asteroid::Render()
{
	if(m_health==0){
		return;
	}
	static Vertex_PCU worldShape[ASTEROID_VERTEX_NUM];
	for (int asteroidIndex=0;asteroidIndex<ASTEROID_VERTEX_NUM;asteroidIndex++){
		worldShape[asteroidIndex]= Vertex_PCU(m_shape[asteroidIndex].m_pos,m_shape[asteroidIndex].m_color,m_shape[asteroidIndex].m_uvTexCoords);
	}
	for( int asteroidIndex=0; asteroidIndex<m_vertexNum; asteroidIndex++ ) {
		Vec3 worldPosition=TransformPosition3DXY( m_shape[asteroidIndex].m_pos, NORMAL_SCALE, m_rotateDegree, m_position );
		worldShape[asteroidIndex].m_pos=worldPosition;
	}
	m_game->m_theRenderer->DrawVertexArray(m_vertexNum,worldShape);
	if(m_game->m_developMode){
		Entity::Render();
	}
	
}

void Asteroid::Die()
{
	m_isDead=true;
}

void Asteroid::CheckHealth()
{
	if(m_health<=0){
		Die();
	}
}

void Asteroid::CheckCollision()
{

}

void Asteroid::CreateAsteroid()
{
	float lastLength=0;
	float lastDegree=0;
	float nextLength=0;
	float nextDegree=0;
	for(int asteroidIndex=0;asteroidIndex<ASTEROID_VERTEX_NUM;asteroidIndex++){
		if(asteroidIndex%3==0){
			m_shape[asteroidIndex]=Vertex_PCU( Vec3( 0.f, 0.f, 0.f ), Rgba8( 100, 100, 100 ), Vec2( 0, 0 ) );
		}
		else if(asteroidIndex%3==1){
			if(lastLength==0){
				lastLength=m_game->m_rng.RollRandomFloatInRange(m_physicsRadius,m_cosmeticRadius);
			}
			Vec2 position= Vec2();
			position.SetPolarDegrees(lastDegree,lastLength);
			m_shape[asteroidIndex]=Vertex_PCU( Vec3(position, 0.f ), Rgba8( 100, 100, 100 ), Vec2( 0, 0 ) );
		}
		else if(asteroidIndex%3==2){
			nextDegree=lastDegree+360.f/16.f;
			nextLength=m_game->m_rng.RollRandomFloatInRange(m_physicsRadius,m_cosmeticRadius);
			Vec2 position1= Vec2();
			position1.SetPolarDegrees(nextDegree,nextLength);
			m_shape[asteroidIndex]=Vertex_PCU( Vec3(position1, 0.f ), Rgba8( 100, 100, 100 ), Vec2( 0, 0 ) );
			//update degrees and length
			lastDegree+=360.f/16.f;
			lastLength=nextLength;
		}
	}
} 

void Asteroid::GenerateDebris()
{
	m_game->SpawnAsteroidDebris(*this);
}

void Asteroid::UpdateRotation(float deltaTime)
{
	m_rotateDegree+=m_angularVelocity*deltaTime;
	ableDebri=false;
}
