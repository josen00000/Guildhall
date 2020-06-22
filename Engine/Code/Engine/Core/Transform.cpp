#include "Transform.hpp"


Transform::Transform( Vec3 pos, float pitch, float roll, float yaw, Vec3 scale, Convention convention )
	:m_pos(pos)
	,m_scale(scale)
{
	m_mat = ToMatrix( convention );
	SetPitchDegrees( pitch );
	SetRollDegrees( roll );
	SetYawDegrees( yaw );
}

Transform::Transform( Convention convention )
{
	m_pos = Vec3::ZERO;
	m_pitchDegrees = 0.f;
	m_rollDegrees = 0.f;
	m_yawDegrees = 0.f;
	m_scale = Vec3::ONE;
	m_mat = ToMatrix( convention );
}

Vec3 Transform::GetForwardDirt( Convention convention ) const
{
	Mat44 transMat = ToMatrix( convention );
	switch( convention )
	{
	case X_FORWARD_Y_LEFT_Z_UP:
		return transMat.GetIBasis3D();
	case X_RIGHT_Y_UP_Z_BACKWARD:
		return -transMat.GetKBasis3D();
	}
	return Vec3::ZERO;
}

Vec3 Transform::GetLeftDirt( Convention convention ) const
{
	Mat44 transMat = ToMatrix( convention );
	switch( convention )
	{
	case X_FORWARD_Y_LEFT_Z_UP:
		return transMat.GetJBasis3D();
	case X_RIGHT_Y_UP_Z_BACKWARD:
		return -transMat.GetIBasis3D();
	}
	return Vec3::ZERO;
}

Vec3 Transform::GetUpDirt( Convention convention ) const
{
	Mat44 transMat = ToMatrix( convention );
	switch( convention )
	{
	case X_FORWARD_Y_LEFT_Z_UP:
		return transMat.GetKBasis3D();
	case X_RIGHT_Y_UP_Z_BACKWARD:
		return transMat.GetJBasis3D();
	}
	return Vec3::ZERO;
}

Mat44 Transform::GetRotationMatrix( Convention convention ) const
{
	Mat44 pitch		= Mat44();
	Mat44 roll		= Mat44();
	Mat44 yaw		= Mat44();
	Mat44 rotation	= Mat44();

	switch( convention )
	{
	case X_FORWARD_Y_LEFT_Z_UP:
		roll	= GetRotationMatrixAlongAxis( Axis_X, m_rollDegrees );
		pitch	= GetRotationMatrixAlongAxis( Axis_Y, m_pitchDegrees );
		yaw		= GetRotationMatrixAlongAxis( Axis_Z, m_yawDegrees );
		break;
	case X_RIGHT_Y_UP_Z_BACKWARD:
		pitch	= GetRotationMatrixAlongAxis( Axis_X, m_pitchDegrees );
		yaw		= GetRotationMatrixAlongAxis( Axis_Y, m_yawDegrees );
		roll	= GetRotationMatrixAlongAxis( Axis_Z, m_rollDegrees );
		break;
	}
	rotation.RotateZDegrees( m_yawDegrees );
	rotation.RotateYDegrees( m_pitchDegrees );
	rotation.RotateXDegrees( m_rollDegrees );
	//Mat44 test = Mat44();
	//test = roll;
	//test.Multiply( pitch );
	//test.Multiply( yaw );
	// Need to debug
	return rotation;
}

Mat44 Transform::GetRotationMatrixAlongAxis( Axis corrAxis, float rotDeg ) const
{
	Mat44 result = Mat44::IDENTITY;

 	switch( corrAxis )
 	{
 	case Axis_X: result.RotateXDegrees( rotDeg ); 
 		break;
 	case Axis_Y: result.RotateYDegrees( rotDeg ); 
 		break;
 	case Axis_Z: result.RotateZDegrees( rotDeg );
 		break;
 	default:
 		break;
 	}
	return result;
}


void Transform::SetPosition( Vec3 pos )
{
	m_pos = pos;
}

void Transform::SetPitchDegrees( float pitch )
{
	m_pitchDegrees = ClampFloat( -90.f, 90.f, pitch );
}

void Transform::SetRollDegrees( float roll )
{
	m_rollDegrees = ClampDegressTo360( roll );
}

void Transform::SetYawDegrees( float yaw )
{
	m_yawDegrees = ClampDegressTo360( yaw );
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

	m_mat = lookat;
	m_mat.Multiply( scale );
}

void Transform::LookAt( Vec3 pos, Vec3 direction, float dist, Vec3 worldUp /*= Vec3( 0.f, 1.f, 0.f ) */ )
{
	m_pos = pos;
	Vec3 target = pos + direction * dist;
	Mat44 lookat = Mat44::GetLookAtMatrix( pos, target, worldUp );
	Mat44 scale = Mat44();
	scale.ScaleNonUniform3D( m_scale );

	m_mat = lookat;
	m_mat.Multiply( scale );
}

void Transform::LookAtStable( Vec3 target, Vec3 worldUp /*= Vec3( 0.f, 1.f, 0.f ) */ )
{
	Vec3 pos = m_pos;
	LookAt( pos, target, worldUp );
}

void Transform::SetMatrix( Mat44 matrix )
{
	m_mat = matrix;
}

void Transform::UpdateMatrix( Convention convention/*=X_RIGHT_Y_UP_Z_BACKWARD*/ )
{
	m_mat = ToMatrix( convention );
}

Mat44 Transform::ToMatrix( Convention convention  ) const
{
	// Calculate Transalation matrix times rotation matrix
	Mat44 translation = Mat44();
	Mat44 rotation = Mat44();
	Mat44 scale = Mat44();
	Mat44 model = Mat44();
	
	scale.ScaleNonUniform3D( m_scale );
	translation.SetTranslation3D( m_pos );
	
	// calculate rotation matrix
	rotation = GetRotationMatrix( convention );

	model = translation;
	model.Multiply( rotation );
	model.Multiply( scale );
	return model;
}

