#pragma once
#include <windows.h>
#include "Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"


//extern HWND g_hWnd;

Camera::Camera( const Vec2& bottomLeft, const Vec2& topRight )
{
	SetOrthoView( bottomLeft, topRight );
}

Camera::~Camera()
{
	if( m_cameraUBO != nullptr ) {
		delete m_cameraUBO;
		m_cameraUBO = nullptr;
	}
}


// Accessor
float Camera::GetCameraHeight() const
{
	return m_outputSize.y;
}

float Camera::GetCameraWidth() const
{
	return m_outputSize.x;
}

float Camera::GetAspectRatio() const
{
	return (m_outputSize.x / m_outputSize.y);
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

AABB2 Camera::GetCameraBox() const
{
	AABB2 result = AABB2( GetOrthoBottomLeft(), GetOrthoTopRight() );
	return result;
}

RenderBuffer* Camera::GetOrCreateCameraBuffer( RenderContext* ctx )
{
	if( m_cameraUBO == nullptr ) {
		m_cameraUBO = new RenderBuffer( ctx, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	}
	UpdateCameraUBO();
	return m_cameraUBO;
}


// Mutator
void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	m_position.x = ( topRight.x + bottomLeft.x ) / 2;
	m_position.y = ( topRight.y + bottomLeft.y ) / 2;
	m_outputSize.x = topRight.x - bottomLeft.x;
	m_outputSize.y = topRight.y - bottomLeft.y;
	m_projection = Mat44::CreateOrthographicProjection( Vec3( bottomLeft, 0.0f ), Vec3( topRight, 1.0f ) );
}

void Camera::SetOutputSize( Vec2 size )
{
	m_outputSize = size;
}

void Camera::SetOutputSize( float aspectRatio, float height )
{
	m_outputSize.x = aspectRatio * height;
	m_outputSize.y = height;
}

void Camera::SetPosition( const Vec3& position )
{
	m_position = position;
	SetOrthoView( GetOrthoBottomLeft(), GetOrthoTopRight() );
}

void Camera::SetProjectionOrthographic( float height, float nearZ /*= -1.0f*/, float farZ /*= 1.0f */ )
{
	UNUSED(nearZ);
	UNUSED(farZ);
	float aspectRatio = GetAspectRatio();
	SetOutputSize( aspectRatio, height );
}

void Camera::SetShouldClearColor( bool shouldClearColor )
{
	m_shouldClearColor = shouldClearColor;
}

void Camera::SetClearMode( unsigned int clearFlags, Rgba8 color, float depth /*= 0.0f */, unsigned int stencil /*= 0 */ )
{
	m_clearMode = clearFlags;
	m_clearColor = color;
	UNUSED( depth );
	UNUSED( stencil );
}

void Camera::SetColorTarget( Texture* colorTarget )
{
	m_colorTarget = colorTarget;
}

void Camera::UpdateCameraUBO()
{
	camera_ortho_t cameraData;
	cameraData.projection = m_projection;
	Mat44 tempTest = Mat44::CreateTranslation3D( m_position );
	cameraData.view = Mat44::CreateTranslation3D( -m_position );
	m_cameraUBO->Update( &cameraData, sizeof( cameraData ), sizeof( cameraData ) );
}

Vec2 Camera::ClientToWorldPosition( Vec2 clientPos )
{
 	Vec2 worldPos;
// 	Vec2 outputDimensions = m_outputSize;
// 	RECT clientRect;
// 	GetClientRect( g_hWnd, &clientRect );
// 	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.bottom, (float)clientRect.right, (float)clientRect.top );
	clientPos.x = ClampZeroToOne( clientPos.x );
	clientPos.y = ClampZeroToOne( clientPos.y );

	AABB2 orthoBounds( GetOrthoBottomLeft(), GetOrthoTopRight() );
	worldPos = orthoBounds.GetPointAtUV( clientPos );
	return worldPos;
}


// Private
Vec2 Camera::GetOrthoMin() const
{
	return ( Vec2( m_position ) - ( m_outputSize / 2 ) );
}

Vec2 Camera::GetOrthoMax() const
{
	return ( Vec2( m_position ) + ( m_outputSize / 2 ) );
}

