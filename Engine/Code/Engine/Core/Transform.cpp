#include "Transform.hpp"


Transform::Transform( Vec3 pos, Vec3 rotation, Vec3 scale )
	:m_pos(pos)
	,m_rotationPRYDegrees(rotation)
	,m_scale(scale)
{
	mat = ToMatrix();
}

Transform::Transform()
{
	m_pos = Vec3::ZERO;
	m_rotationPRYDegrees = Vec3::ZERO;
	m_scale = Vec3::ONE;
	mat = ToMatrix();
}

Mat44 Transform::GetRotationMatrix() const
{
	Mat44 pitch		= Mat44();
	Mat44 roll		= Mat44();
	Mat44 yaw		= Mat44();
	Mat44 rotation	= Mat44();

	pitch.RotateXDegrees( m_rotationPRYDegrees.x );
	roll.RotateZDegrees( m_rotationPRYDegrees.y );
	yaw.RotateYDegrees( m_rotationPRYDegrees.z );

	rotation = yaw;
	rotation.Multiply( roll );
	rotation.Multiply( pitch );
	return rotation;
}

void Transform::SetPosition( Vec3 pos )
{
	m_pos = pos;
}

void Transform::SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw )
{
	pitch = ClampFloat( -90.f, 90.f, pitch );
	roll = ClampDegressTo360( roll );
	yaw = ClampDegressTo360( yaw );
	//roll = RangeMapFloat( 0.f, 360.f, -180.f, 180.f, roll );
	//yaw = RangeMapFloat( 0.f, 360.f, -180.f, 180.f, yaw );
	//roll = ClampFloat( -180.f, 180.f, roll );
	//yaw = ClampFloat( -180.f, 180.f, yaw );
	m_rotationPRYDegrees = Vec3( pitch, roll, yaw );
}

void Transform::SetRotationFromPitchRollYawDegrees( Vec3 rotation )
{
	SetRotationFromPitchRollYawDegrees( rotation.x, rotation.y, rotation.z );
}

void Transform::SetScale( Vec3 scale )
{
	m_scale = scale;
}

void Transform::LookAt( Vec3 pos, Vec3 target, Vec3 worldUp /*= Vec3( 0.f, 1.f, 0.f ) */ )
{
	m_pos = pos;
	Mat44 lookat = Mat44::GetLookAtMatrix( pos, target, worldUp );
	Mat44 scale = Mat44();
	scale.ScaleNonUniform3D( m_scale ); 

	mat = lookat;
	mat.Multiply( scale );
}

void Transform::LookAt( Vec3 pos, Vec3 direction, float dist, Vec3 worldUp /*= Vec3( 0.f, 1.f, 0.f ) */ )
{
	m_pos = pos;
	Vec3 target = pos + direction * dist;
	Mat44 lookat = Mat44::GetLookAtMatrix( pos, target, worldUp );
	Mat44 scale = Mat44();
	scale.ScaleNonUniform3D( m_scale );

	mat = lookat;
	mat.Multiply( scale );
}

void Transform::LookAtStable( Vec3 target, Vec3 worldUp /*= Vec3( 0.f, 1.f, 0.f ) */ )
{
	Vec3 pos = m_pos;
	LookAt( pos, target, worldUp );
}

void Transform::SetMatrix( Mat44 matrix )
{
	mat = matrix;
}

void Transform::UpdateMatrix()
{
	mat = ToMatrix();
}

Mat44 Transform::ToMatrix() const
{
	// Calculate Transalation matrix times rotation matrix
	Mat44 translation = Mat44();
	Mat44 rotation = Mat44();
	Mat44 scale = Mat44();
	Mat44 model = Mat44();
	
	scale.ScaleNonUniform3D( m_scale );
	translation.SetTranslation3D( m_pos );
	
	// calculate rotation matrix
	rotation = GetRotationMatrix();

	model = translation;
	model.Multiply( rotation );
	model.Multiply( scale );
	return model;
}
