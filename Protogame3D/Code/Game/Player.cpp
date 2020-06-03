#include "Player.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"


extern RenderContext* g_theRenderer;
extern InputSystem* g_theInputSystem;
extern Game* g_theGame;


Player::Player()
	:Entity(Vec2(1.5,1.5),ENTITY_TYPE_PLAYER, ENTITY_FACTION_EVIL)
{
	m_angularVelocity = 180.f;
	m_physicsRadius = 0.2f;
	m_cosmeticRadius=0.5f;
	CreateTank();
}

Player::~Player()
{
	delete m_tankTexture;
	delete m_barrelTexture;
	m_tankTexture=nullptr;
	m_barrelTexture=nullptr;
}

void Player::Update( float deltaTime )
{
	UpdateTank(deltaTime);
	UpdateBarrel(deltaTime);
}

 void Player::Render()const
{
	RenderTank();
	RenderBarrel();
	if(g_theGame->m_developMode){
		Entity::Render();
	}
}

void Player::Die()
{

}

void Player::TakeDamage()
{

}

void Player::UpdateTank( float deltaSeconds )
{
	//Check the input
	const XboxController xboxController = g_theInputSystem->GetXboxController(0);
	float magnitude = 0.f;
	float turningDegrees = 0.f;
	float turnedDegrees = 0.f;
	while( m_orientationDegrees > 360.f ) {
		m_orientationDegrees -= 360.f;
	}
	while( m_orientationDegrees < -0.f ) {
		m_orientationDegrees += 360.f;
	}
	if(xboxController.isConnected()){
		//using xbox input
		AnalogJoystick leftJoyStick = xboxController.GetLeftJoystick();
		magnitude = leftJoyStick.GetMagnitude();
		if(magnitude > 0){
			turningDegrees = leftJoyStick.GetAngleDegrees();
			while(turningDegrees > 360.f){
				turningDegrees -= 360.f;
			}
			while(turningDegrees <- 0.f){
				turningDegrees += 360.f;
			}
		}
	}
	else{
		//using keyboard input
		if(g_theInputSystem->IsKeyDown(KEYBOARD_BUTTON_ID_E)){
			magnitude = 1.f;
			turningDegrees = 90.f;
		}	
		else{
			magnitude = 0.f;
			turningDegrees = 0.f;
		}
	}
	if(magnitude > 0){
		if( fabsf( turningDegrees-m_orientationDegrees ) > MAX_TURNING_DIFFERENCE ) {
			turnedDegrees = GetTurnedToward( m_orientationDegrees, turningDegrees, m_angularVelocity*deltaSeconds );
			
		}
		else{
			turnedDegrees = turningDegrees;
		}

		m_orientationDegrees = turnedDegrees;
		
	}
	m_velocity.SetPolarDegrees(m_orientationDegrees,magnitude);
	Entity::Update(deltaSeconds);
}

void Player::UpdateBarrel( float deltaSeconds )
{
	/*const XboxController xboxController=g_theInputSystem->GetXboxController( 0 );
	float magnitude=0.f;
	float turningDegrees=0.f;
	if(xboxController.isConnected()){
		AnalogJoystick rightJoyStick=xboxController.GetRightJoystick();
	}*/
	UNUSED(deltaSeconds);
}

void Player::RenderTank() const
{
	const std::vector<Vertex_PCU> m_tankShapeInWorld=TransformVertexArray(m_tankVertices,1,m_orientationDegrees,m_position);
	const std::vector<Vertex_PCU> m_barrelShapeInWorld=TransformVertexArray(m_barrelVertices,1,m_orientationDegrees,m_position);
	
	g_theRenderer->BindTexture(m_tankTexture);
	g_theRenderer->DrawVertexVector(m_tankShapeInWorld);
	g_theRenderer->BindTexture( m_barrelTexture );
	g_theRenderer->DrawVertexVector( m_barrelShapeInWorld);
}

void Player::RenderBarrel() const
{

}

void Player::CreateTank()
{	//emplace_back(constructor arguments directly)
	m_tankShape = AABB2(Vec2(-.5f,-.5f),Vec2(.5f,.5f));
	m_tankVertices.push_back(Vertex_PCU(Vec3(m_tankShape.mins,0),Rgba8(255,255,255),Vec2(0,0)));
	m_tankVertices.push_back(Vertex_PCU(Vec3(m_tankShape.mins.x,m_tankShape.maxs.y,0),Rgba8(255,255,255),Vec2(0,1)));
	m_tankVertices.push_back(Vertex_PCU(Vec3(m_tankShape.maxs,0),Rgba8(255,255,255),Vec2(1,1)));
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs.x, m_tankShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
	m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

	m_barrelShape = AABB2(Vec2(-.5f,-.5f),Vec2(.5f,.5f));
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.mins.x, m_barrelShape.maxs.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 1 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.maxs.x, m_barrelShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
	m_barrelVertices.push_back( Vertex_PCU( Vec3( m_barrelShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

	m_tankTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
	m_barrelTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankTop.png");
}
