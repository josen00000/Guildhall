#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"


Camera::Camera( const Vec2& bottomLeft, const Vec2& topRight )
{
	SetOrthoView( bottomLeft, topRight );
	//SetProjectionPerspective()
}

void Camera::Translate( const Vec3& translation )
{

}

float Camera::GetCameraHeight() const
{
	return m_outputSize.y;
}

float Camera::GetCameraWidth() const
{
	return m_outputSize.x;
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
	m_position.x = ( topRight.x + bottomLeft.x ) / 2;
	m_position.y = ( topRight.y + bottomLeft.y ) / 2;
	m_outputSize.x = topRight.x - bottomLeft.x;
	m_outputSize.y = topRight.y - bottomLeft.y;
	m_projection = Mat44::CreateOrthographicProjectionMatrix( Vec3( bottomLeft, 0.0f ), Vec3( topRight, 1.0f ) );
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	Vec2 bottomLeft;
	bottomLeft.x = m_position.x - ( m_outputSize.x / 2 );
	bottomLeft.y = m_position.y - ( m_outputSize.y / 2 );
	return bottomLeft;
}

Vec2 Camera::GetOrthoTopRight() const
{
	Vec2 topRight;
	topRight.x = m_position.x + (m_outputSize.x / 2 );
	topRight.y = m_position.y + (m_outputSize.y / 2 );
	return topRight;
}

Vec3 Camera::GetPosition() const
{
	return m_position;
}

AABB2 Camera::GetCameraAsBox() const
{
	AABB2 cameraBox = AABB2( GetOrthoBottomLeft(), GetOrthoTopRight() );
	return cameraBox;
}

void Camera::SetPosition( const Vec3& position )
{
	m_position = position;
	m_transform.SetPosition( position );
	UpdateModelMatrix();	
	SetOrthoView( GetOrthoBottomLeft(), GetOrthoTopRight() );
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

void Camera::SetProjectionPerspective( float fov, float nearZ, float farZ )
{
	float height = 1.f / (float)tan( fov * 0.5 );
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
	m_projection.m_values[Mat44::Kw] = nearZ * farZ * q;
	// T
	m_projection.m_values[Mat44::Tx] = 0.f;
	m_projection.m_values[Mat44::Ty] = 0.f;
	m_projection.m_values[Mat44::Tz] = -1.f;
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
	Mat44 tempTest = Mat44::CreateTranslation3D( m_position );
	cameraData.view = Mat44::CreateTranslation3D( -m_position );
	//cameraData.view = m_view; NEED test
	//cameraData.view = Mat44::IDENTITY;
	m_cameraUBO->Update( &cameraData, sizeof( cameraData ), sizeof( cameraData ) );
}

Vec3 Camera::ClientTOWorld( Vec2 client, float ndcZ )
{
	return Vec3::ZERO;
}

Vec3 Camera::WorldToClient( Vec3 worldPos )
{
	return Vec3::ZERO;
}

void Camera::UpdateModelMatrixFromTranslation()
{
	m_model = m_transform.ToMatrix();
}

void Camera::UpdateViewMatrix()
{
	// same as invertOrthonormal
	m_view = m_model;
	m_view.SetTranslation3D( Vec3::ZERO );
	m_view.TransposeMatrix();

	Vec3 translation = m_model.GetTranslation3D();
	Vec3 inverse_translation = m_view.TransformPosition3D( -translation );
	m_view.SetTranslation3D( inverse_translation );

}

void Camera::UpdateModelMatrix()
{
	m_model	 = m_transform.ToMatrix();
}

void Camera::SetShouldClearColor( bool shouldClearColor )
{
	m_shouldClearColor = shouldClearColor;
}

