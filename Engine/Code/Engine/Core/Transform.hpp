#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/Convention.hpp"

enum Axis {
	Axis_X,
	Axis_Y,
	Axis_Z
};

class Transform {
public:
	Transform(){}
	~Transform(){}
	explicit Transform( Convention convention );
	explicit Transform( Vec3 pos, float pitch, float roll, float yaw, Vec3 scale, Convention convention );
public:
	// Accessor
	Vec3 GetPosition() const { return m_pos; }
	float GetPitchDegrees() const { return m_pitchDegrees; }
	float GetRollDegrees() const { return m_rollDegrees; }
	float GetYawDegrees() const { return m_yawDegrees; }
	Vec3 GetScale() const { return m_scale; }
	Vec3 GetForwardDirt( Convention convention ) const;

	Mat44 GetRotationMatrix( Convention convention=X_RIGHT_Y_UP_Z_BACKWARD ) const;
	Mat44 GetMatrix() const { return m_mat; } // temp using for DebugRender
	Mat44 GetRotationMatrixAlongAxis( Axis corrAxis, float rotDeg ) const;

	// Mutator
	void SetPosition( Vec3 pos );
	void SetPitchDegrees( float pitch );
	void SetRollDegrees( float roll );
	void SetYawDegrees( float yaw );
	void SetScale( Vec3 scale );
	void LookAt( Vec3 pos, Vec3 target, Vec3 worldUp = Vec3( 0.f, 1.f, 0.f ) );
	void LookAt( Vec3 pos, Vec3 direction, float dist, Vec3 worldUp = Vec3( 0.f, 1.f, 0.f ) );
	void LookAtStable( Vec3 target, Vec3 worldUp = Vec3( 0.f, 1.f, 0.f ) );
	void SetMatrix( Mat44 matrix ); // temp using for debug render
	void UpdateMatrix( Convention convention=X_RIGHT_Y_UP_Z_BACKWARD );
	Mat44 ToMatrix( Convention convention=X_RIGHT_Y_UP_Z_BACKWARD ) const;

public:
	Vec3 m_pos					= Vec3::ZERO;
	float m_pitchDegrees		= 0; 
	float m_rollDegrees			= 0; 
	float m_yawDegrees			= 0; 
	Vec3 m_scale				= Vec3::ONE;
	Mat44 m_mat;
};