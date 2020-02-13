#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"


// the bot_left and top_right seems to be redundant
//
// 


Camera::Camera(const Camera& camera)
	:m_position(camera.m_position)
	,m_height(camera.m_height)
	,m_width(camera.m_width)
	,m_AABB2(camera.m_AABB2)
	,m_projection(camera.m_projection)
{
}


Camera::Camera( const Vec2& bottomLeft, const Vec2& topRight )
{
	m_position.x=(topRight.x-bottomLeft.x)/2;
	m_position.y=(topRight.y-bottomLeft.y)/2;
	m_height=topRight.y-bottomLeft.y;
	m_width=topRight.x-bottomLeft.x;
	SetOrthoView( bottomLeft, topRight );

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
	m_AABB2.mins = bottomLeft;
	m_AABB2.maxs = topRight;

	m_projection = Mat44::CreateOrthographicProjection( Vec3( bottomLeft, 0.0f ), Vec3( topRight, 1.0f ) );
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_AABB2.mins;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return m_AABB2.maxs;
}

Vec3 Camera::GetPosition() const
{
	return m_position;
}

void Camera::SetPosition( const Vec2 inPosition )
{
	m_position=inPosition;
	UpdateCamera();

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

void Camera::UpdateCamera()
{
	m_AABB2.mins.x=m_position.x-m_width/2;
	m_AABB2.mins.y=m_position.y-m_height/2;

	m_AABB2.maxs.x=m_position.x+m_width/2;
	m_AABB2.maxs.y=m_position.y+m_height/2;
}

void Camera::UpdateCameraUBO()
{
	camera_ortho_t cameraData;
	//cameraData.orthoMax = camera.GetOrthoTopRight();
	//cameraData.orthoMin = camera.GetOrthoBottomLeft();
	cameraData.projection = m_projection; 
	Mat44 tempTest = Mat44::CreateTranslation3D( m_position );
	cameraData.view = Mat44::CreateTranslation3D( -m_position );
		//Mat44::CreateOrthographicProjection( Vec3( GetOrthoBottomLeft(), 0.f ), Vec3( GetOrthoTopRight(), 1.f ));
	m_cameraUBO->Update( &cameraData, sizeof( cameraData ), sizeof( cameraData ) );
}

void Camera::SetShouldClearColor( bool shouldClearColor )
{
	m_shouldClearColor = shouldClearColor;
}

