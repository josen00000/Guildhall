#pragma once
#include<Engine/Renderer/Camera.hpp>

// the bot_left and top_right seems to be redundant
//
// 


Camera::Camera(const Camera& camera)
	:m_position(camera.m_position)
	,m_height(camera.m_height)
	,m_width(camera.m_width)
	,m_AABB2(camera.m_AABB2)
{
}


Camera::Camera( const Vec2& bottomLeft, const Vec2& topRight )
{
	m_position.x=(topRight.x-bottomLeft.x)/2;
	m_position.y=(topRight.y-bottomLeft.y)/2;
	m_height=topRight.y-bottomLeft.y;
	m_width=topRight.x-bottomLeft.x;
	m_AABB2.mins=bottomLeft;
	m_AABB2.maxs=topRight;
}

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight )
{
	m_AABB2.mins=bottomLeft;
	m_AABB2.maxs=topRight;
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_AABB2.mins;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return m_AABB2.maxs;
}

Vec2 Camera::GetPosition() const
{
	return m_position;
}

void Camera::SetPosition( const Vec2 inPosition )
{
	m_position=inPosition;
	UpdateCamera();

}

void Camera::UpdateCamera()
{
	m_AABB2.mins.x=m_position.x-m_width/2;
	m_AABB2.mins.y=m_position.y-m_height/2;

	m_AABB2.maxs.x=m_position.x+m_width/2;
	m_AABB2.maxs.y=m_position.y+m_height/2;
}


