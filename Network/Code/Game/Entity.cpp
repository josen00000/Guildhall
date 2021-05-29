#include "Entity.hpp"
#include "Game/Game.hpp"
#include "Game/Network/Server.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Renderer/DebugRender.hpp"

extern Game* g_theGame;
extern RenderContext* g_theRenderer;
extern Server* g_theServer;

Entity::Entity()
{
}

Entity::Entity( const EntityDefinition& entityDef )
{
	m_definition = &entityDef;
	m_cylinder.SetStartPos( Vec3( m_2Dpos, 0 ) );
	m_cylinder.SetEndPos( Vec3( m_2Dpos, 1 ) );
	float radius = m_definition->m_radius;
	m_cylinder.SetRadius( radius );
}

void Entity::Update( float deltaSeconds )
{
	if( m_isDead ){ return; }
	if( m_isMoving ) {
		Vec2 forwardNormal = Vec2( 1.f, 0.f );
		forwardNormal.SetAngleDegrees( m_orientation );
		Vec2 moveDirtWorld = Vec2( ( forwardNormal.x * m_moveDirt.x ) + ( -forwardNormal.y * m_moveDirt.y ), ( forwardNormal.y * m_moveDirt.x ) + ( forwardNormal.x * m_moveDirt.y ) );
		float walkSpeed = m_definition->m_walkSpeed;
		Vec2 position = m_2Dpos + moveDirtWorld * walkSpeed * deltaSeconds;
		Set2DPos( position );
	}
	if( !m_isPlayer ) {
		std::string test = m_definition->m_billboardType;
		BillboardMode billboardMode = GetBillboardModeWithString( m_definition->m_billboardType );
		UpdateTexCoords( g_theGame->m_gameCamera, billboardMode, g_theGame->m_convension );
	}
}


void Entity::UpdateVerts( Vec2 uvAtMin, Vec2 uvAtMax )
{
	if( !m_isPlayer ) {
		std::string test = m_definition->m_billboardType;
		BillboardMode billboardMode = GetBillboardModeWithString( m_definition->m_billboardType );
		UpdateTexCoords( g_theGame->m_gameCamera, billboardMode, g_theGame->m_convension );
	}

	m_verts.clear();
	float halfWidth;
	float halfHeight;
	if( m_definition == nullptr ) {
		halfWidth = 0.75f;
		halfHeight = 0.75f;
	}
	else {
		halfWidth = m_definition->m_size.x / 2;
		halfHeight = m_definition->m_size.y / 2;
	}
	Vec3 leftDownPos	= Vec3( m_2Dpos, halfHeight ) + m_texLeft * halfWidth - m_texUp * halfHeight;
	Vec3 leftUpPos		= Vec3( m_2Dpos, halfHeight ) + m_texLeft * halfWidth + m_texUp * halfHeight;
	Vec3 rightDownPos	= Vec3( m_2Dpos, halfHeight ) - m_texLeft * halfWidth - m_texUp * halfHeight;
	Vec3 rightUpPos		= Vec3( m_2Dpos, halfHeight ) - m_texLeft * halfWidth + m_texUp * halfHeight;

	// right down triangle
	m_verts.emplace_back( rightDownPos, Rgba8::WHITE, uvAtMin  );
	m_verts.emplace_back( leftDownPos, Rgba8::WHITE, Vec2( uvAtMax.x, uvAtMin.y ) );
	m_verts.emplace_back( rightUpPos, Rgba8::WHITE, Vec2( uvAtMin.x, uvAtMax.y ) );

	// left up triangle
	m_verts.emplace_back( rightUpPos, Rgba8::WHITE, Vec2( uvAtMin.x, uvAtMax.y ) );
	m_verts.emplace_back( leftDownPos, Rgba8::WHITE, Vec2( uvAtMax.x, uvAtMin.y ) );
	m_verts.emplace_back( leftUpPos, Rgba8::WHITE, uvAtMax );
}

void Entity::UpdateVerts( float deltaSeconds )
{

}

void Entity::Shoot()
{
}

void Entity::GetShot()
{
	m_hp -= 10;
	if( m_hp <= 0 ) {
		m_isDead = true;
	}
}

void Entity::UpdateTexCoords( const Camera* camera, BillboardMode billboardMode, Convention convention )
{
 	Vec3 cameraPos = camera->GetPosition();
 	Vec3 cameraForward = camera->GetForwardDirt( convention );
	switch( billboardMode )
	{
	case BILLBOARD_MODE_CAMERA_FACING_XY:
		m_texForward = cameraPos - GetPosition();
		m_texForward.z = 0;
		m_texForward.Normalize();
		m_texLeft = Vec3( -m_texForward.y, m_texForward.x, 0.f );
		m_texUp = Vec3( 0.f, 0.f, 1.f );
		break;
	case BILLBOARD_MODE_CAMERA_OPPOSING_XY:
		m_texForward = -Vec3( cameraForward.x, cameraForward.y, 0 );
		m_texForward.Normalize();
		m_texLeft = Vec3( -m_texForward.y, m_texForward.x, 0.f );
		m_texUp = Vec3( 0.f, 0.f, 1.f );
		break;
	case BILLBOARD_MODE_CAMERA_FACING_XYZ:
		m_texForward = cameraPos - GetPosition();
		m_texForward.Normalize();
		m_texLeft = CrossProduct3D( Vec3( 0.f, 0.f, 1.f ), m_texForward );
		m_texLeft.Normalize();
		m_texUp = CrossProduct3D( m_texForward, m_texLeft );
		break;
	case BILLBOARD_MODE_CAMERA_OPPOSING_XYZ:
		m_texForward = -cameraForward;
		m_texLeft = -camera->GetLeftDirt( convention );
		m_texUp = camera->GetUpDirt( convention );
		break;
	default:
		break;
	}
}

void Entity::Render() const
{
	if( m_isDead ){ return; }
	Texture* texture = m_definition->m_spriteTexture;
	/*texture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );*/
	g_theRenderer->SetDiffuseTexture( texture );
	g_theRenderer->DrawVertexVector( m_verts );
	if( g_theGame->m_debugMode && !m_isPlayer ) {
		g_theRenderer->SetFillMode( RASTER_FILL_WIREFRAME );
		g_theRenderer->SetDiffuseTexture( nullptr );
		g_theRenderer->DrawCylinder( m_cylinder, 8, Rgba8::CYAN );
		Vec3 startPos = GetPosition();
		Vec2 startPos2D = Get2DPosition();
		Vec2 forwardNormal2D = GetforwardNormal2D();
		
		LineSegment2 tempLine = LineSegment2( startPos2D, startPos2D + forwardNormal2D );
		g_theRenderer->DrawLineWithHeight( tempLine, startPos.z, 0.05f, Rgba8::WHITE );
		g_theRenderer->SetFillMode( RASTER_FILL_SOLID );
	}
	
		RenderHealth();
}

void Entity::RenderHealth() const
{
	std::string health = "health" + std::to_string( m_hp );
	Vec3 pos = Vec3( m_2Dpos + Vec2( 0.f, 0.3f ), 0.8f );
	Transform wordTrans;
	wordTrans.SetPosition( pos );
	wordTrans.SetPitchDegrees( 0.f );
	wordTrans.SetRollDegrees( 90.f );
	wordTrans.SetYawDegrees( 90.f );
	wordTrans.SetScale( Vec3( 0.1f, 0.1f, 0.1f ) );
	if( m_isPlayer ) {
		DebugAddScreenLeftAlignTextf( 0.8f, 0.5f, Vec2::ZERO, Rgba8::RED, "Health: %i", m_hp );
	}
	else {
		DebugAddWorldText( wordTrans, Vec2::ZERO, Rgba8::GREEN, Rgba8::GREEN, 0.016f, DEBUG_RENDER_ALWAYS, health );	
	}
}

Vec2 Entity::GetforwardNormal2D() const
{
	Vec2 result = Vec2( 1.f, 0 );
	result.SetAngleDegrees( m_orientation );
	return result;
}

void Entity::SetCanBePushedByWalls( bool isAble )
{
	m_canBePushedByWalls = isAble;
}

void Entity::SetCanBePushedByEntities( bool isAble )
{
	m_canBePushedByEntities = isAble;
}

void Entity::SetCanPushEntities( bool isAble )
{
	m_canPushEntities = isAble;
}

void Entity::SetIsMoving( bool isMoving )
{
	m_isMoving = isMoving;
}

void Entity::SetIsPlayer( bool isPlayer )
{
	m_isPlayer = isPlayer;
}

void Entity::SetMass( float mass )
{
	m_mass = mass;
}

void Entity::Set2DPos( Vec2 pos )
{
	m_2Dpos = pos;
	m_cylinder.SetStartPos( Vec3( m_2Dpos, 0 ) );
	float height = m_definition->m_height;
	m_cylinder.SetEndPos( Vec3( m_2Dpos, height ) );
}

void Entity::SetOrientation( float orientation )
{
	while( orientation > 360.f ) {
		orientation -= 360.f;
	}
	while( orientation < 0.f ) {
		orientation += 360.f;
	}
	m_orientation = orientation;
}

void Entity::SetMoveDirt( Vec2 moveDirt )
{
	m_moveDirt = moveDirt;
}

void Entity::SetHP( int hp )
{
	m_hp = hp;
}

void Entity::SetIsDead( bool isDead )
{
	m_isDead = isDead;
}

void Entity::SetTexForward( Vec3 texForward )
{
	m_texForward = texForward;
}

void Entity::SetTexLeft( Vec3 texLeft )
{
	m_texLeft = texLeft;
}

void Entity::SetTexUp( Vec3 texUp )
{
	m_texUp = texUp;
}

BillboardMode Entity::GetBillboardModeWithString( std::string billboardText )
{
	if( billboardText.compare( "CameraFacingXY" ) ) { return BILLBOARD_MODE_CAMERA_FACING_XY; }
	else if( billboardText.compare( "CameraFacingXY" ) ) { return BILLBOARD_MODE_CAMERA_FACING_XY; }
	else if( billboardText.compare( "CameraFacingXYZ" ) ) { return BILLBOARD_MODE_CAMERA_FACING_XYZ; }
	else if( billboardText.compare( "CameraOpposingXY" ) ) { return BILLBOARD_MODE_CAMERA_OPPOSING_XY; }
	else if( billboardText.compare( "CameraOpposingXYZ" ) ) { return BILLBOARD_MODE_CAMERA_OPPOSING_XYZ; }
	else {
		ERROR_RECOVERABLE( Stringf( " wrong billboardText: %s ", billboardText.c_str() ) );
		return NUM_BILLBOARD_MODES;
	}
}
