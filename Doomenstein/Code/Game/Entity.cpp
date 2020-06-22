#include "Entity.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"


Entity::Entity()
{
	m_cylinder.SetStartPos( Vec3( m_2Dpos, 0 ) );
	m_cylinder.SetEndPos( Vec3( m_2Dpos, 1 ) );
}

void Entity::UpdateVerts()
{
	m_verts.clear();
	Vec3 leftDownPos	= Vec3( m_2Dpos, 0.5f ) + m_texLeft * 0.3f - m_texUp * 0.5f;
	Vec3 leftUpPos		= Vec3( m_2Dpos, 0.5f ) + m_texLeft * 0.3f + m_texUp * 0.5f;
	Vec3 rightDownPos	= Vec3( m_2Dpos, 0.5f ) - m_texLeft * 0.3f - m_texUp * 0.5f;
	Vec3 rightUpPos		= Vec3( m_2Dpos, 0.5f ) - m_texLeft * 0.3f + m_texUp * 0.5f;

	// right down triangle
	m_verts.emplace_back( rightDownPos, Rgba8::WHITE, Vec2( 1.f, 0.f ) );
	m_verts.emplace_back( leftDownPos, Rgba8::WHITE, Vec2::ZERO );
	m_verts.emplace_back( rightUpPos, Rgba8::WHITE, Vec2::ONE );

	// left up triangle
 	m_verts.emplace_back( rightUpPos, Rgba8::WHITE, Vec2::ONE );
 	m_verts.emplace_back( leftDownPos, Rgba8::WHITE, Vec2::ZERO );
 	m_verts.emplace_back( leftUpPos, Rgba8::WHITE, Vec2( 0.f, 1.f ) );
}

void Entity::UpdateTexCoords( const Camera* camera, BillboardMode billboardMode, Convention convention )
{
// 	Vec3 entityForward = Vec3::ZERO;
// 	Vec3 entityPos = m_billBoardEntity->GetPosition();
// 	switch( billboardMode )
// 	{
// 	case BILLBOARD_MODE_CAMERA_FACING_XY:
// 		entityForward = cameraPos - entityPos;
// 		entityForward.z = 0;
// 		break;
// 	case BILLBOARD_MODE_CAMERA_OPPOSING_XY:
// 		entityForward = -cameraForward;
// 		entityForward.z = 0;
// 		break;
// 	case BILLBOARD_MODE_CAMERA_FACING_XYZ:
// 		entityForward = cameraPos - entityPos;
// 		break;
// 	case BILLBOARD_MODE_CAMERA_OPPOSING_XYZ:
// 		break;
// 	case NUM_BILLBOARD_MODES:
// 		break;
// 	default:
// 		break;
// 	}
// 	entityForward.Normalize();
// 	SetTexForward( texFord );

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

void Entity::SetMass( float mass )
{
	m_mass = mass;
}

void Entity::Set2DPos( Vec2 pos )
{
	m_2Dpos = pos;
	m_cylinder.SetStartPos( Vec3( m_2Dpos, 0 ) );
	m_cylinder.SetEndPos( Vec3( m_2Dpos, 1 ) );
}

void Entity::SetOrientation( float orientation )
{
	m_orientation = orientation;
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
