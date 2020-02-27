#pragma once
#include "Engine/Math/Vec3.hpp"

struct Mat44;


class Transform {
public:
	Transform(){}
	~Transform(){}
	Transform( Vec3 pos, Vec3 rotation, Vec3 scale );
public:
	void SetPosition( Vec3 pos );
	void SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw );
	void SetScale( Vec3 scale );

	Mat44 ToMatrix() const;

public:
	Vec3 m_pos					= Vec3::ZERO;
	Vec3 m_rotationPRYDegrees	= Vec3::ZERO;
	Vec3 m_scale				= Vec3::ZERO;
};