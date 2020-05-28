#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"

enum RotateOrder {
	PITCH_ROLL_YAW_ORDER,
	PITCH_YAW_ROLL_ORDER,
	ROLL_PITCH_YAW_ORDER,
	ROLL_YAW_PITCH_ORDER,
	YAW_PITCH_ROLL_ORDER,
	YAW_ROLL_PITCH_ORDER,
	NUM_ORDER
};


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
	Mat44 GetRotationMatrix( RotateOrder order = PITCH_ROLL_YAW_ORDER ) const;
	Mat44 GetMatrix() const { return mat; } // temp using for DebugRender

	// Mutator
	void SetPosition( Vec3 pos );
	void SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw );
	void SetRotationFromPitchRollYawDegrees( Vec3 rotation );
	void SetScale( Vec3 scale );
	void LookAt( Vec3 pos, Vec3 target, Vec3 worldUp = Vec3( 0.f, 1.f, 0.f ) );
	void LookAt( Vec3 pos, Vec3 direction, float dist, Vec3 worldUp = Vec3( 0.f, 1.f, 0.f ) );
	void LookAtStable( Vec3 target, Vec3 worldUp = Vec3( 0.f, 1.f, 0.f ) );
	void SetMatrix( Mat44 matrix ); // temp using for debug render
	void UpdateMatrix();
	Mat44 ToMatrix() const;
public:
	Vec3 m_pos					= Vec3::ZERO;
	// better set pitch roll yaw seperately.
	Vec3 m_rotationPRYDegrees	= Vec3::ZERO;
	Vec3 m_scale				= Vec3::ONE;
	Mat44 mat;
	RotateOrder m_rotOrder		= PITCH_ROLL_YAW_ORDER;
};