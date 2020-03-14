#pragma once
#include "Engine/Math/Vec3.hpp"

struct Mat44;


class Transform {
public:
	Transform();
	~Transform(){}
	Transform( Vec3 pos, Vec3 rotation, Vec3 scale );
public:
	// Accessor
	Vec3 GetPosition() const { return m_pos; }
	Vec3 GetRotationPRYDegrees() const { return m_rotationPRYDegrees; }
	Vec3 GetScale() const { return m_scale; }
	Mat44 GetRotationMatrix() const;

	// Mutator
	void SetPosition( Vec3 pos );
	void SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw );
	void SetRotationFromPitchRollYawDegrees( Vec3 rotation );
	void SetScale( Vec3 scale );

	Mat44 ToMatrix() const;

public:
	Vec3 m_pos					= Vec3::ZERO;
	Vec3 m_rotationPRYDegrees	= Vec3::ZERO;
	Vec3 m_scale				= Vec3::ONE;
};