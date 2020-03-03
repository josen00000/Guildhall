#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"


Camera::Camera( const Vec2& bottomLeft, const Vec2& topRight )
{
	SetOrthoView( bottomLeft, topRight );
	m_projectionType = PROJECTION_ORTHOGRAPHIC;
}

Camera::Camera( float fov/*=60.f*/, float nearZ/*=-0.1*/, float farZ/*=-100*/, Vec3 pos, Vec3 rotPRY, Vec3 scale )
{
	m_transform = Transform( pos, rotPRY, scale );
	SetProjectionPerspective( fov, nearZ, farZ );
	m_projectionType = PROJECTION_PERSPECTIVE;
}

Camera* Camera::CreateOrthographicCamera( const Vec2& bottomLeft, const Vec2& topRight )
{
	Camera* result = new Camera( bottomLeft, topRight );
	return result;
}

Camera* Camera::CreatePerspectiveCamera( float fov, float nearZ, float farZ, Vec3 pos, Vec3 rotPRY, Vec3 scale )
{
	Camera* result = new Camera( fov, nearZ, farZ, pos, rotPRY, scale );
	return result;
}

void Camera::Translate( const Vec3& translation )
{

}

float Camera::GetCameraHeight() const
{
	return m_dimension.y;
}

float Camera::GetCameraWidth() const
{
	return m_dimension.x;
}

Camera::~Camera()
{
	if( m_cameraUBO != nullptr ) {
		delete m_cameraUBO;
		m_cameraUBO = nullptr;
	}
}

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	Vec3 position;
	position.x = ( topRight.x + bottomLeft.x ) / 2;
	position.y = ( topRight.y + bottomLeft.y ) / 2;
	position.z = 1;
	SetPosition( position );
	m_dimension.x = topRight.x - bottomLeft.x;
	m_dimension.y = topRight.y - bottomLeft.y;
	m_projection = Mat44::CreateOrthographicProjectionMatrix( Vec3( bottomLeft, 0.0f ), Vec3( topRight, 1.0f ) );
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	Vec2 bottomLeft;
	bottomLeft.x = GetPosition().x - ( m_dimension.x / 2 );
	bottomLeft.y = GetPosition().y - ( m_dimension.y / 2 );
	return bottomLeft;
}

Vec2 Camera::GetOrthoTopRight() const
{
	Vec2 topRight;
	topRight.x = GetPosition().x + (m_dimension.x / 2 );
	topRight.y = GetPosition().y + (m_dimension.y / 2 );
	return topRight;
}

Vec3 Camera::GetPosition() const
{
	return m_transform.GetPosition();
}

AABB2 Camera::GetCameraAsBox() const
{
	AABB2 cameraBox = AABB2( GetOrthoBottomLeft(), GetOrthoTopRight() );
	return cameraBox;
}

Mat44 Camera::GetModelMatrix() const
{
	return m_transform.ToMatrix();
}

void Camera::SetPosition( const Vec3& position )
{
	m_transform.SetPosition( position );
}

void Camera::SetClearMode( unsigned int clearFlags, Rgba8 color, float depth /*= 0.0f */, unsigned int stencil /*= 0 */ )
{
	m_clearMode = clearFlags;
	m_clearColor = color;
	UNUSED(depth);
	UNUSED(stencil);
}

void Camera::SetColorTarget( Texture* colorTarget )
{
	m_colorTarget = colorTarget;
}

void Camera::SetDepthStencilTarget( Texture* texture )
{

}

void Camera::SetPitchRollYawRotation( float pitch, float roll, float yaw )
{
		
}

void Camera::SetProjectionPerspective( float fov/*=60*/, float nearZ/*=-0.1*/, float farZ/*=-100 */ )
{
	float fovRadians = ConvertDegreesToRadians( fov );
	float height = 1.f / (float)tan( fovRadians * 0.5 );
	float zRange = farZ - nearZ;
	float q = 1.0f / zRange;
	float aspect = 1.f;

	// I
	m_projection.m_values[Mat44::Ix] = height / aspect;
	m_projection.m_values[Mat44::Iy] = 0.f;
	m_projection.m_values[Mat44::Iz] = 0.f;
	m_projection.m_values[Mat44::Iw] = 0.f;
	// J
	m_projection.m_values[Mat44::Jx] = 0.f;
	m_projection.m_values[Mat44::Jy] = height;
	m_projection.m_values[Mat44::Jz] = 0.f;
	m_projection.m_values[Mat44::Jw] = 0.f;
	// K
	m_projection.m_values[Mat44::Kx] = 0.f;
	m_projection.m_values[Mat44::Ky] = 0.f;
	m_projection.m_values[Mat44::Kz] = -farZ * q;
	m_projection.m_values[Mat44::Kw] = -1;
	// T
	m_projection.m_values[Mat44::Tx] = 0.f;
	m_projection.m_values[Mat44::Ty] = 0.f;
	m_projection.m_values[Mat44::Tz] = nearZ * farZ * q;
	m_projection.m_values[Mat44::Tw] = 0.f;
}

RenderBuffer* Camera::GetOrCreateCameraBuffer( RenderContext* ctx )
{
	if( m_cameraUBO == nullptr ) {
		m_cameraUBO = new RenderBuffer( ctx, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	}
	UpdateCameraUBO();
	return m_cameraUBO;
	
}

void Camera::UpdateCameraUBO()
{
	camera_ortho_t cameraData;
	cameraData.projection = m_projection; 
	UpdateViewMatrix();
	cameraData.view = m_view;
	m_cameraUBO->Update( &cameraData, sizeof( cameraData ), sizeof( cameraData ) );
}

void Camera::UpdateCameraRotation( Vec3 deltaRot )
{
	Vec3 cameraRot = m_transform.GetRotationPRYDegrees();
	cameraRot += deltaRot;
	m_transform.SetRotationFromPitchRollYawDegrees( cameraRot );
}

Vec3 Camera::ClientTOWorld( Vec2 client, float ndcZ )
{
	return Vec3::ZERO;
}

Vec3 Camera::WorldToClient( Vec3 worldPos )
{
	return Vec3::ZERO;
}

void Camera::UpdateViewMatrix()
{
	// same as invertOrthonormal
	m_view = GetModelMatrix();
	m_view.SetTranslation3D( Vec3::ZERO );
	m_view.TransposeMatrix();

	Vec3 translation = m_transform.GetPosition();
	Vec3 inverse_translation = m_view.TransformPosition3D( -translation );
	m_view.SetTranslation3D( -translation );
}

void Camera::SetShouldClearColor( bool shouldClearColor )
{
	m_shouldClearColor = shouldClearColor;
}

