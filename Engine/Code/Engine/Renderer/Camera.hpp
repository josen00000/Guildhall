#pragma once
#include<Engine//Math/vec2.hpp>
#include<Engine/Math/AABB2.hpp>
class Camera{

public:
	Camera(){}
	~Camera(){}
	Camera(const Camera& camera);
	explicit Camera(const Vec2& bottomLeft, const Vec2& topRight);
public:
	 void SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight);
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	Vec2 GetPosition() const;
	void SetPosition(const Vec2 inPosition);
	void UpdateCamera();
public:

	AABB2 m_AABB2=AABB2(Vec2(0,0),Vec2(0,0));
	Vec2 m_position=Vec2(0.f,0.f);
	float m_width=0.f;
	float m_height=0.f;
};
