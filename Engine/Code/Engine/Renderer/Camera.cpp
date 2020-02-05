#pragma once
#include <windows.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"

// the bot_left and top_right seems to be redundant
//
// 

extern HWND g_hWnd;

Camera::Camera(const Camera& camera)
	:m_position(camera.m_position)
	,m_outputSize(camera.m_outputSize)
{
}


Camera::Camera( const Vec2& bottomLeft, const Vec2& topRight )
{
	SetOrthoView( bottomLeft, topRight );
}

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	m_position.x = ( topRight.x - bottomLeft.x ) / 2;
	m_position.y = ( topRight.y - bottomLeft.y ) / 2;
	m_position.z = 0.f;
	m_outputSize = Vec2( ( topRight.x - bottomLeft.x ), ( topRight.y - bottomLeft.y ) );
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	Vec2 pos = Vec2( m_position );
	return pos - ( m_outputSize / 2 );
}

Vec2 Camera::GetOrthoTopRight() const
{
	Vec2 pos = Vec2( m_position);
	return pos + ( m_outputSize / 2 );
}

Vec2 Camera::GetPosition() const
{
	return m_position;
}

float Camera::GetHeight() const
{
	return m_outputSize.y;
}

float Camera::GetWidth() const
{
	return m_outputSize.x;
}

float Camera::GetAspectRatio() const
{
	return ( m_outputSize.x / m_outputSize.y );
}

void Camera::SetPosition( const Vec3& position )
{
	m_position = position;
}

void Camera::SetProjectionOrthographic( float height, float nearZ /*= -1.0f*/, float farZ /*= 1.0f */ )
{
	UNUSED(nearZ);
	UNUSED(farZ);
	float aspectRatio = GetAspectRatio();
	SetOutputSize( height, aspectRatio );
}

Vec2 Camera::ClientToWorldPosition( Vec2 clientPos )
{
	Vec2 worldPos;
	Vec2 outputDimensions = m_outputSize;
	RECT clientRect;
	GetClientRect( g_hWnd, &clientRect );
	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.bottom, (float)clientRect.right, (float)clientRect.top );
	Vec2 normalizedPos = clientBounds.GetUVForPoint( clientPos );
	normalizedPos.x = ClampZeroToOne( normalizedPos.x );
	normalizedPos.y = ClampZeroToOne( normalizedPos.y );
	
	AABB2 orthoBounds( GetOrthoBottomLeft(), GetOrthoTopRight() );
	worldPos = orthoBounds.GetPointAtUV( normalizedPos );
	// for now, you have an orthoMin and orthoMax.
	// so...
	// 1. Convert clientPos to pixel coordinates to normalized coordinates (0, 1) 
	//    by range mapping them from your client size to (0,1) in both dimensions.
	// 2. Convert to your ortho position by rangemapping the normalized coordinate 
	//    from (0,1) to (min, max).

	// note: you could skip and just RangeMap from (clientSpace to orthoSpace), but the additional
	// step is good practice for something coming up in SD2

	// note 2: client space is y-down, but your world space is y-up, be sure to 
	// take this into account

	// These TODOs are notes on things that will change in this function
	// as MP2 & SD2 advance;
	// TODO - take into account render target
	//        clientPos being the pixel location on the texture

	// TODO - use projection matrix to compute this

	// TODO - Support ndc-depth paramater for 3D-coordinates, needed for ray casts.

	// TODO - take into account viewport

	return worldPos;
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

Vec2 Camera::GetOrthoMin() const
{
	return ( Vec2( m_position ) - ( m_outputSize / 2 ) );
}

Vec2 Camera::GetOrthoMax() const
{
	return ( Vec2( m_position ) + ( m_outputSize / 2 ) );
}

