#include "PlayerClient.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Game/Network/Server.hpp"
#include "Game/EntityFactory.hpp"
#include "Game/Network/Server.hpp"
#include "Game/Game.hpp"

extern InputSystem*		g_theInputSystem;
extern AudioSystem*		g_theAudioSystem;
extern RenderContext*	g_theRenderer;
extern DevConsole*		g_theConsole;

PlayerClient::~PlayerClient()
{

}

PlayerClient::PlayerClient( Server* owner, Camera* camera )
	:Client::Client(owner)
{
	m_gameCamera = camera;
}

void PlayerClient::Startup()
{
	m_entity = m_owner->CreateAndPushPlayer();
}

void PlayerClient::Shutdown()
{
	
}

void PlayerClient::Update( float deltaSeconds )
{
	Vec3 playerPos = m_entity->GetPosition();
	m_gameCamera->SetPosition( playerPos );
}

void PlayerClient::BeginFrame()
{
	g_theInputSystem->BeginFrame();
	PreUpdatePlayer();
}

void PlayerClient::EndFrame()
{
	Render();
}

void PlayerClient::HandleUserInput()
{

}

void PlayerClient::PreUpdatePlayer()
{
	static Vec3 testPos = Vec3::ZERO;
	static Vec3 forward = Vec3::ZERO;
	static float maxDist = 0.f;
	if( m_entity ) {
		testPos = m_entity->GetPosition();
		forward = m_gameCamera->GetForwardDirt( m_owner->GetGameConvention() );
		maxDist = 1.f;
	}
	
	if( !m_entity ) { return; }

	float baseRotSpeed = 30.f;
	float playerOrientation = m_entity->m_orientation;

	Vec2 mouseMove = g_theInputSystem->GetRelativeMovementPerFrame();
	float deltaPlayerOrientation = -mouseMove.x * baseRotSpeed;

	playerOrientation += deltaPlayerOrientation;
	m_gameCamera->m_transform.SetYawDegrees( playerOrientation );

	bool isMoving = false;
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_E ) ) {
		isMoving = true;
		m_entity->SetMoveDirt( Vec2( 1.f, 0.f ) );
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
		isMoving = true;
		m_entity->SetMoveDirt( Vec2( -1.f, 0.f ) );
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_F ) ) {
		isMoving = true;
		m_entity->SetMoveDirt( Vec2( 0.f, -1.f ) );
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {
		isMoving = true;
		m_entity->SetMoveDirt( Vec2( 0.f, 1.f ) );
	}
	m_entity->SetIsMoving( isMoving );
	m_entity->SetOrientation( playerOrientation );
// 	m_player->Update( deltaSeconds );
// 
// 	m_gameCamera->SetPosition( m_player->GetPosition() ); // TODO Move to somewhere correct

	//Map* currentMap = m_world->GetCurrentMap();
	//currentMap->RayCast( m_player->GetPosition() + m_gameCamera->GetForwardDirt( m_convension ) * m_player->GetRadius() * 1.5f, m_gameCamera->GetForwardDirt( m_convension ), 1.f );
}

void PlayerClient::Render()
{
	Rgba8 tempColor = Rgba8::DARK_GRAY;
	m_gameCamera->m_clearColor = tempColor;
	Texture* backBuffer = g_theRenderer->GetSwapChainBackBuffer();
	m_gameCamera->SetColorTarget( backBuffer, 0 );

	Convention convention = m_owner->GetGameConvention();
	g_theRenderer->BeginCamera( m_gameCamera, convention );
	g_theRenderer->SetBlendMode( BlendMode::BLEND_ALPHA );
	g_theRenderer->EnableDepth( COMPARE_DEPTH_LESS, true );
	g_theRenderer->SetCullMode( RASTER_CULL_BACK );
	g_theRenderer->SetTintColor( Rgba8::WHITE );
	g_theRenderer->BindShader( "Data/Shader/WorldOpaque.hlsl" );

	
	// Render map
	std::vector<Vertex_PCU> mapRenderData = m_owner->m_game->GetMapRenderData();
	Texture* mapRenderTexture = m_owner->m_game->GetMapTexture();

	g_theRenderer->EnableDepth( COMPARE_DEPTH_LESS, true );
	g_theRenderer->SetDiffuseTexture( mapRenderTexture );
	g_theRenderer->DrawVertexVector( mapRenderData );

	// Render Entity
	for( int i = 0; i < m_owner->m_entities.size(); i++ ) {
		m_owner->m_entities[i]->Render();
	}

// 	g_theRenderer->SetCullMode( RASTER_CULL_NONE );
// 	g_theRenderer->DisableDepth();
// 	g_theRenderer->BindShader( static_cast<Shader*>(nullptr) );
// 	g_theRenderer->SetDiffuseTexture( nullptr );


	g_theRenderer->EndCamera();

 	RenderUI();
// 
// 	DebugRenderWorldToCamera( m_gameCamera, m_convension );
// 	DebugRenderScreenTo( m_gameCamera->GetColorTarget() );
// 	
	// render dev console
	g_theConsole->Render( *g_theRenderer );
}

void PlayerClient::HandleInput( std::string input )
{
	UNUSED(input);
}

void PlayerClient::RenderUI()
{
	m_UICamera->SetDepthStencilTarget( nullptr ); // don't need depth stencil target set target to nullptr
	m_UICamera->SetClearMode( CLEAR_NONE );
	m_UICamera->SetUseDepth( false );
	m_UICamera->SetColorTarget( m_gameCamera->GetColorTarget() );

	g_theRenderer->BeginCamera( m_UICamera, X_RIGHT_Y_UP_Z_BACKWARD );
	RenderBaseHud();
	RenderGun();
	g_theRenderer->EndCamera();
}

void PlayerClient::RenderBaseHud()
{
	Texture* hudTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Hud_Base.png" );
	g_theRenderer->SetDiffuseTexture( hudTexture );
	AABB2 hudBox = AABB2( Vec2::ZERO, Vec2( 160.f, 11.7f ) );
	g_theRenderer->DrawAABB2D( hudBox, Rgba8::WHITE );
}

void PlayerClient::RenderGun()
{
	Texture* gunTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/ViewModelsSpriteSheet_8x8.png" );
	g_theRenderer->SetDiffuseTexture( gunTexture );
	Vertex_PCU vertices[6];
	Vec2 gunUVAtMax;
	Vec2 gunUVAtMin;
	SpriteSheet* viewModelSpriteSheet = m_owner->m_game->GetViewModelSpriteSheet();
	viewModelSpriteSheet->GetSpriteUVs( gunUVAtMin, gunUVAtMax, IntVec2( 0, 0 ) );
	vertices[0] = Vertex_PCU( Vec3( 55.f, 11.7f, 0.f ), Rgba8::WHITE, gunUVAtMin );
	vertices[1] = Vertex_PCU( Vec3( 105.f, 11.7f, 0.f ), Rgba8::WHITE, Vec2( gunUVAtMax.x, gunUVAtMin.y ) );
	vertices[2] = Vertex_PCU( Vec3( 105.f, 61.7f, 0.f ), Rgba8::WHITE, gunUVAtMax );

	vertices[3] = Vertex_PCU( Vec3( 55.f, 11.7f, 0.f ), Rgba8::WHITE, gunUVAtMin );
	vertices[4] = Vertex_PCU( Vec3( 105.f, 61.7f, 0.f ), Rgba8::WHITE, gunUVAtMax );
	vertices[5] = Vertex_PCU( Vec3( 55.f, 61.7f, 0.f ), Rgba8::WHITE, Vec2( gunUVAtMin.x, gunUVAtMax.y ) );

	g_theRenderer->DrawVertexArray( 6, vertices );
}

void PlayerClient::RequestServerCreatePlayer()
{
	m_owner->CreateAndPushPlayer();
}

void PlayerClient::PlayAudioWithSoundID( size_t id )
{
	g_theAudioSystem->PlaySound( id );
}

void PlayerClient::SetUICamera( Camera* camera )
{
	m_UICamera = camera;
}
