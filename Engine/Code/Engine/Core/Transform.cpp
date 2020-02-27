#include "Transform.hpp"
#include "Engine/Math/Mat44.hpp"


Transform::Transform( Vec3 pos, Vec3 rotation, Vec3 scale )
	:m_pos(pos)
	,m_rotationPRYDegrees(rotation)
	,m_scale(scale)
{
}

void Transform::SetPosition( Vec3 pos )
{
	m_pos = pos;
}

void Transform::SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw )
{
	pitch = ClampFloat( -90.f, 90.f, pitch );
	roll = ClampFloat( -180.f, 180.f, roll );
	yaw = ClampFloat( -180.f, 180.f, yaw );
	m_rotationPRYDegrees = Vec3( pitch, roll, yaw );
}

void Transform::SetScale( Vec3 scale )
{
	m_scale = scale;
}

Mat44 Transform::ToMatrix() const
{
	// Calculate Transalation matrix times rotation matrix
	Mat44 translation = Mat44();
	Mat44 pitch = Mat44();
	Mat44 roll = Mat44();
	Mat44 yaw = Mat44();
	Mat44 rotation = Mat44();
	Mat44 scale = Mat44();
	Mat44 model = Mat44();
	
	scale.ScaleNonUniform3D( m_scale );
	pitch.RotateXDegrees( m_rotationPRYDegrees.x );
	roll.RotateZDegrees( m_rotationPRYDegrees.y );
	yaw.RotateYDegrees( m_rotationPRYDegrees.z );
	translation.SetTranslation3D( m_pos );
	
	// calculate rotation matrix
	rotation = yaw; 
	rotation.Multiply( roll );
	rotation.Multiply( pitch );

	model = translation;
	model.Multiply( rotation );
	model.Multiply( scale );
	return model;
}
