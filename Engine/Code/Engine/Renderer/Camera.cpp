#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"


Camera::Camera( const Vec2& bottomLeft, const Vec2& topRight )
{
	SetOrthoView( bottomLeft, topRight );
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
	m_projection = Mat44::CreateOrthographicProjection( Vec3( bottomLeft, 0.0f ), Vec3( topRight, 1.0f ) );
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
	Mat44 tempTest = Mat44::CreateTranslation3D( m_position );
	cameraData.view = Mat44::CreateTranslation3D( -m_position );
	m_cameraUBO->Update( &cameraData, sizeof( cameraData ), sizeof( cameraData ) );
}

void Camera::SetShouldClearColor( bool shouldClearColor )
{
	m_shouldClearColor = shouldClearColor;
}

