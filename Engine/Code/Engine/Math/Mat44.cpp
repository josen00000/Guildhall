#include "Mat44.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"


Mat44 Mat44::IDENTITY = Mat44();


Mat44::Mat44()
{
	//I
	m_values[Ix] = 1;
	m_values[Iy] = 0;
	m_values[Iz] = 0;
	m_values[Iw] = 0;
	//J			   
	m_values[Jx] = 0;
	m_values[Jy] = 1;
	m_values[Jz] = 0;
	m_values[Jw] = 0;
	//K			   
	m_values[Ky] = 0;
	m_values[Kx] = 0;
	m_values[Kz] = 1;
	m_values[Kw] = 0;
	//T			   
	m_values[Tx] = 0;
	m_values[Ty] = 0;
	m_values[Tz] = 0;
	m_values[Tw] = 1;
}

Mat44::Mat44( float* sixteenValuesBasisMajor )
{
/*
	//I
	m_values[Ix] = sixteenValuesBasisMajor[0];
	m_values[Iy] = sixteenValuesBasisMajor[1];
	m_values[Iz] = sixteenValuesBasisMajor[2];
	m_values[Iw] = sixteenValuesBasisMajor[3];
	//J			  
	m_values[Jx] = sixteenValuesBasisMajor[4];
	m_values[Jy] = sixteenValuesBasisMajor[5];
	m_values[Jz] = sixteenValuesBasisMajor[6];
	m_values[Jw] = sixteenValuesBasisMajor[7];
	//K			   
	m_values[Kx] = sixteenValuesBasisMajor[8];
	m_values[Ky] = sixteenValuesBasisMajor[9];
	m_values[Kz] = sixteenValuesBasisMajor[10];
	m_values[Kw] = sixteenValuesBasisMajor[11];
	//T			   
	m_values[Tx] = sixteenValuesBasisMajor[12];
	m_values[Ty] = sixteenValuesBasisMajor[13];
	m_values[Tz] = sixteenValuesBasisMajor[14];
	m_values[Tw] = sixteenValuesBasisMajor[15];

*/

	

	for(int index = 0; index < 16; index++){
		m_values[index] = sixteenValuesBasisMajor[index];
	}

}

Mat44::Mat44( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
{
	//I
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Iz] = 0;
	m_values[Iw] = 0;
	//J			   
	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Jz] = 0;
	m_values[Jw] = 0;
	//K			   
	m_values[Ky] = 0;
	m_values[Kx] = 0;
	m_values[Kz] = 1;
	m_values[Kw] = 0;
	//T			   
	m_values[Tx] = translation2D.x;
	m_values[Ty] = translation2D.y;
	m_values[Tz] = 0;
	m_values[Tw] = 1;
	



}

Mat44::Mat44( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
{
	//I
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;
	m_values[Iw] = 0;
	//J
	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;
	m_values[Jw] = 0;
	//K
	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;
	m_values[Kw] = 0;
	//T
	m_values[Tx] = translation3D.x;
	m_values[Ty] = translation3D.y;
	m_values[Tz] = translation3D.z;
	m_values[Tw] = 1;
}

Mat44::Mat44( const Vec4& iBasisHomogeneous, const Vec4& jBasisHomogeneous, const Vec4& kBasisHomogeneous, const Vec4& translationHomogeneous )
{
	//I
	m_values[Ix] = iBasisHomogeneous.x;
	m_values[Iy] = iBasisHomogeneous.y;
	m_values[Iz] = iBasisHomogeneous.z;
	m_values[Iw] = iBasisHomogeneous.w;
	//J
	m_values[Jx] = jBasisHomogeneous.x;
	m_values[Jy] = jBasisHomogeneous.y;
	m_values[Jz] = jBasisHomogeneous.z;
	m_values[Jw] = jBasisHomogeneous.w;
	//K
	m_values[Kx] = kBasisHomogeneous.x;
	m_values[Ky] = kBasisHomogeneous.y;
	m_values[Kz] = kBasisHomogeneous.z;
	m_values[Kw] = kBasisHomogeneous.w;
	//T
	m_values[Tx] = translationHomogeneous.x;
	m_values[Ty] = translationHomogeneous.y;
	m_values[Tz] = translationHomogeneous.z;
	m_values[Tw] = translationHomogeneous.w;
}


const Vec2 Mat44::TransformVector2D( const Vec2& vectorQuantitiy ) const
{
	Vec2 result;
	result.x = m_values[Ix] * vectorQuantitiy.x + m_values[Jx] * vectorQuantitiy.y;
	result.y = m_values[Iy] * vectorQuantitiy.x + m_values[Jy] * vectorQuantitiy.y;
	return result;
}

const Vec3 Mat44::TransformVector3D( const Vec3& vectorQuantitiy ) const
{
	Vec3 result;
	result.x = m_values[Ix] * vectorQuantitiy.x + m_values[Jx] * vectorQuantitiy.y + m_values[Kx] * vectorQuantitiy.z;
	result.y = m_values[Iy] * vectorQuantitiy.x + m_values[Jy] * vectorQuantitiy.y + m_values[Ky] * vectorQuantitiy.z;
	result.z = m_values[Iz] * vectorQuantitiy.x + m_values[Jz] * vectorQuantitiy.y + m_values[Kz] * vectorQuantitiy.z;
	return result;
}

const Vec2 Mat44::TransformPosition2D( const Vec2& position ) const
{
	Vec2 result;
	result.x = m_values[Ix] * position.x + m_values[Jx] * position.y + m_values[Tx];
	result.y = m_values[Iy] * position.x + m_values[Jy] * position.y + m_values[Ty];
	return result;
}

const Vec3 Mat44::TransformPosition3D( const Vec3& position ) const
{
	Vec3 result;
	result.x = m_values[Ix] * position.x + m_values[Jx] * position.y + m_values[Kx] * position.z + m_values[Tx];
	result.y = m_values[Iy] * position.x + m_values[Jy] * position.y + m_values[Ky] * position.z + m_values[Ty];
	result.z = m_values[Iz] * position.x + m_values[Jz] * position.y + m_values[Kz] * position.z + m_values[Tz];
	return result;

}

const Vec4 Mat44::TransformHomogeneousPoint3D( const Vec4& homogeneousPoint ) const
{
	Vec4 result;
	result.x = m_values[Ix] * homogeneousPoint.x + m_values[Jx] * homogeneousPoint.y + m_values[Kx] * homogeneousPoint.z + m_values[Tx] * homogeneousPoint.w;
	result.y = m_values[Iy] * homogeneousPoint.x + m_values[Jy] * homogeneousPoint.y + m_values[Ky] * homogeneousPoint.z + m_values[Ty] * homogeneousPoint.w;
	result.z = m_values[Iz] * homogeneousPoint.x + m_values[Jz] * homogeneousPoint.y + m_values[Kz] * homogeneousPoint.z + m_values[Tz] * homogeneousPoint.w;
	result.w = m_values[Iw] * homogeneousPoint.x + m_values[Jw] * homogeneousPoint.y + m_values[Kw] * homogeneousPoint.z + m_values[Tw] * homogeneousPoint.w;
	return result;
}

const Vec2 Mat44::GetIBasis2D() const
{
	Vec2 result;
	result.x = m_values[Ix];
	result.y = m_values[Iy];
	return result;
}

const Vec2 Mat44::GetJBasis2D() const
{
	Vec2 result;
	result.x = m_values[Jx];
	result.y = m_values[Jy];
	return result;
}

const Vec2 Mat44::GetTranslation2D() const
{
	Vec2 result;
	result.x = m_values[Tx];
	result.y = m_values[Ty];
	return result;
}

const Vec3 Mat44::GetIBasis3D() const
{
	Vec3 result;
	result.x = m_values[Ix];
	result.y = m_values[Iy];
	result.z = m_values[Iz];
	return result;
}

const Vec3 Mat44::GetJBasis3D() const
{
	Vec3 result;
	result.x = m_values[Jx];
	result.y = m_values[Jy];
	result.z = m_values[Jz];
	return result;
}

const Vec3 Mat44::GetKBasis3D() const
{
	Vec3 result;
	result.x = m_values[Kx];
	result.y = m_values[Ky];
	result.z = m_values[Kz];
	return result;
}

const Vec3 Mat44::GetTranslation3D() const
{
	Vec3 result;
	result.x = m_values[Tx];
	result.y = m_values[Ty];
	result.z = m_values[Tz];
	return result;
}

const Vec4 Mat44::GetIBasis4D() const
{
	Vec4 result;
	result.x = m_values[Ix];
	result.y = m_values[Iy];
	result.z = m_values[Iz];
	result.w = m_values[Iw];
	return result;
}

const Vec4 Mat44::GetJBasis4D() const
{
	Vec4 result;
	result.x = m_values[Jx];
	result.y = m_values[Jy];
	result.z = m_values[Jz];
	result.w = m_values[Jw];
	return result;
}

const Vec4 Mat44::GetKBasis4D() const
{
	Vec4 result;
	result.x = m_values[Kx];
	result.y = m_values[Ky];
	result.z = m_values[Kz];
	result.w = m_values[Kw];
	return result;
}

const Vec4 Mat44::GetTranslation4D() const
{
	Vec4 result;
	result.x = m_values[Tx];
	result.y = m_values[Ty];
	result.z = m_values[Tz];
	result.w = m_values[Tw];
	return result;
}

void Mat44::SetTranslation2D( const Vec2& translation2D )
{
	m_values[Tx] = translation2D.x;
	m_values[Ty] = translation2D.y;
}

void Mat44::SetTranslation3D( const Vec3& translation3D )
{
	m_values[Tx] = translation3D.x;
	m_values[Ty] = translation3D.y;
	m_values[Tz] = translation3D.z;

}

void Mat44::SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D )
{
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;

	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
}

void Mat44::SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
{
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;

	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;

	m_values[Tx] = translation2D.x;
	m_values[Ty] = translation2D.y;
}

void Mat44::SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D )
{
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;

	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;

	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;
}

void Mat44::SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
{
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;

	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;

	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;

	m_values[Tx] = translation3D.x;
	m_values[Ty] = translation3D.y;
	m_values[Tz] = translation3D.z;

}

void Mat44::SetBasisVectors4D( const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D )
{
	m_values[Ix] = iBasis4D.x;
	m_values[Iy] = iBasis4D.y;
	m_values[Iz] = iBasis4D.z;
	m_values[Iw] = iBasis4D.w;

	m_values[Jx] = jBasis4D.x;
	m_values[Jy] = jBasis4D.y;
	m_values[Jz] = jBasis4D.z;
	m_values[Jw] = jBasis4D.w;

	m_values[Kx] = kBasis4D.x;
	m_values[Ky] = kBasis4D.y;
	m_values[Kz] = kBasis4D.z;
	m_values[Kw] = kBasis4D.w;

	m_values[Tx] = translation4D.x;
	m_values[Ty] = translation4D.y;
	m_values[Tz] = translation4D.z;
	m_values[Tw] = translation4D.w;
}

void Mat44::RotateXDegrees( float degreesAboutX )
{
	float cosDegrees = CosDegrees(degreesAboutX);
	float sinDegrees = SinDegrees(degreesAboutX);
	Mat44 temRotate;
	temRotate.m_values[Jy] = cosDegrees;
	temRotate.m_values[Jz] = sinDegrees;
	temRotate.m_values[Ky] = -sinDegrees;
	temRotate.m_values[Kz] = cosDegrees;

	Multiply( temRotate );
}

void Mat44::RotateYDegrees( float degreesAboutY )
{
	float cosDegrees = CosDegrees( degreesAboutY );
	float sinDegrees = SinDegrees( degreesAboutY );
	Mat44 temRotate;
	temRotate.m_values[Ix] = cosDegrees;
	temRotate.m_values[Iz] = -sinDegrees;
	temRotate.m_values[Kx] = sinDegrees;
	temRotate.m_values[Kz] = cosDegrees;

	Multiply( temRotate );

}

void Mat44::RotateZDegrees( float degreesAboutZ )
{
	float cosDegrees = CosDegrees( degreesAboutZ );
	float sinDegrees = SinDegrees( degreesAboutZ );
	Mat44 temRotate;
	temRotate.m_values[Ix] = cosDegrees;
	temRotate.m_values[Iy] = sinDegrees;
	temRotate.m_values[Jx] = -sinDegrees;
	temRotate.m_values[Jy] = cosDegrees;

	Multiply( temRotate );

}

void Mat44::Translate2D( const Vec2& translationXY )
{
	Mat44 temTranslate;
	temTranslate.m_values[Tx] = translationXY.x;
	temTranslate.m_values[Ty] = translationXY.y;

	Multiply(temTranslate);
}

void Mat44::Translate3D( const Vec3& translation3D )
{
	Mat44 temTranslate;
	temTranslate.m_values[Tx] = translation3D.x;
	temTranslate.m_values[Ty] = translation3D.y;
	temTranslate.m_values[Tz] = translation3D.z;

	Multiply( temTranslate );
}

void Mat44::ScaleUniform2D( float uniformScaleXY )
{
	Mat44 temScale;
	temScale.m_values[Ix] = uniformScaleXY;
	temScale.m_values[Jy] = uniformScaleXY;

	Multiply(temScale);
}

void Mat44::ScaleNonUniform2D( const Vec2& ScaleFactorsXY )
{
	Mat44 temScale;
	temScale.m_values[Ix] = ScaleFactorsXY.x;
	temScale.m_values[Jy] = ScaleFactorsXY.y;

	Multiply( temScale );
}

void Mat44::ScaleUniform3D( float uniformScaleXYZ )
{
	Mat44 temScale;
	temScale.m_values[Ix] = uniformScaleXYZ;
	temScale.m_values[Jy] = uniformScaleXYZ;
	temScale.m_values[Kz] = uniformScaleXYZ;

	Multiply( temScale );

}

void Mat44::ScaleNonUniform3D( const Vec3& scaleFactorsXYZ )
{
	Mat44 temScale;
	temScale.m_values[Ix] = scaleFactorsXYZ.x;
	temScale.m_values[Jy] = scaleFactorsXYZ.y;
	temScale.m_values[Kz] = scaleFactorsXYZ.z;

	Multiply( temScale );

}

void Mat44::Multiply( const Mat44& arbitraryTransFormationToAppend )
{
	Mat44 tem = arbitraryTransFormationToAppend;
	Mat44 temResult;
	// temResult I
	temResult.m_values[Ix] = m_values[Ix] * tem.m_values[Ix] + m_values[Jx] * tem.m_values[Iy] + m_values[Kx] * tem.m_values[Iz] + m_values[Tx] * tem.m_values[Iw];
	temResult.m_values[Iy] = m_values[Iy] * tem.m_values[Ix] + m_values[Jy] * tem.m_values[Iy] + m_values[Ky] * tem.m_values[Iz] + m_values[Ty] * tem.m_values[Iw];
	temResult.m_values[Iz] = m_values[Iz] * tem.m_values[Ix] + m_values[Jz] * tem.m_values[Iy] + m_values[Kz] * tem.m_values[Iz] + m_values[Tz] * tem.m_values[Iw];
	temResult.m_values[Iw] = m_values[Iw] * tem.m_values[Ix] + m_values[Jw] * tem.m_values[Iy] + m_values[Kw] * tem.m_values[Iz] + m_values[Tw] * tem.m_values[Iw];
	// temResult J
	temResult.m_values[Jx] = m_values[Ix] * tem.m_values[Jx] + m_values[Jx] * tem.m_values[Jy] + m_values[Kx] * tem.m_values[Jz] + m_values[Tx] * tem.m_values[Jw];
	temResult.m_values[Jy] = m_values[Iy] * tem.m_values[Jx] + m_values[Jy] * tem.m_values[Jy] + m_values[Ky] * tem.m_values[Jz] + m_values[Ty] * tem.m_values[Jw];
	temResult.m_values[Jz] = m_values[Iz] * tem.m_values[Jx] + m_values[Jz] * tem.m_values[Jy] + m_values[Kz] * tem.m_values[Jz] + m_values[Tz] * tem.m_values[Jw];
	temResult.m_values[Jw] = m_values[Iw] * tem.m_values[Jx] + m_values[Jw] * tem.m_values[Jy] + m_values[Kw] * tem.m_values[Jz] + m_values[Tw] * tem.m_values[Jw];
	// temResult K
	temResult.m_values[Kx] = m_values[Ix] * tem.m_values[Kx] + m_values[Jx] * tem.m_values[Ky] + m_values[Kx] * tem.m_values[Kz] + m_values[Tx] * tem.m_values[Kw];
	temResult.m_values[Ky] = m_values[Iy] * tem.m_values[Kx] + m_values[Jy] * tem.m_values[Ky] + m_values[Ky] * tem.m_values[Kz] + m_values[Ty] * tem.m_values[Kw];
	temResult.m_values[Kz] = m_values[Iz] * tem.m_values[Kx] + m_values[Jz] * tem.m_values[Ky] + m_values[Kz] * tem.m_values[Kz] + m_values[Tz] * tem.m_values[Kw];
	temResult.m_values[Kw] = m_values[Iw] * tem.m_values[Kx] + m_values[Jw] * tem.m_values[Ky] + m_values[Kw] * tem.m_values[Kz] + m_values[Tw] * tem.m_values[Kw];
	// temResult T
	temResult.m_values[Tx] = m_values[Ix] * tem.m_values[Tx] + m_values[Jx] * tem.m_values[Ty] + m_values[Kx] * tem.m_values[Tz] + m_values[Tx] * tem.m_values[Tw];
	temResult.m_values[Ty] = m_values[Iy] * tem.m_values[Tx] + m_values[Jy] * tem.m_values[Ty] + m_values[Ky] * tem.m_values[Tz] + m_values[Ty] * tem.m_values[Tw];
	temResult.m_values[Tz] = m_values[Iz] * tem.m_values[Tx] + m_values[Jz] * tem.m_values[Ty] + m_values[Kz] * tem.m_values[Tz] + m_values[Tz] * tem.m_values[Tw];
	temResult.m_values[Tw] = m_values[Iw] * tem.m_values[Tx] + m_values[Jw] * tem.m_values[Ty] + m_values[Kw] * tem.m_values[Tz] + m_values[Tw] * tem.m_values[Tw];

	*this = temResult;
}



// static method
const Mat44 Mat44::CreateXRotationDegrees( float degreesAboutX )
{		    
	float cosDegrees = CosDegrees( degreesAboutX );
	float sinDegrees = SinDegrees( degreesAboutX );

	Mat44 temResult;
	temResult.m_values[Jy] = cosDegrees;
	temResult.m_values[Jz] = sinDegrees;
	temResult.m_values[Ky] = -sinDegrees;
	temResult.m_values[Kz] = cosDegrees;

	return temResult;
}		    
		    
const Mat44 Mat44::CreateYRotationDegrees( float degreesAboutY )
{		    

	float cosDegrees = CosDegrees( degreesAboutY );
	float sinDegrees = SinDegrees( degreesAboutY );

	Mat44 temResult;
	temResult.m_values[Ix] = cosDegrees;
	temResult.m_values[Iz] = -sinDegrees;
	temResult.m_values[Kx] = sinDegrees;
	temResult.m_values[Kz] = cosDegrees;

	return temResult;
}		    
		    
const Mat44 Mat44::CreateZRotationDegrees( float degreesAboutZ )
{		    

	float cosDegrees = CosDegrees( degreesAboutZ );
	float sinDegrees = SinDegrees( degreesAboutZ );

	Mat44 temResult;
	temResult.m_values[Ix] = cosDegrees;
	temResult.m_values[Iy] = sinDegrees;
	temResult.m_values[Jx] = -sinDegrees;
	temResult.m_values[Jy] = cosDegrees;

	return temResult;
}		    
		    
const Mat44 Mat44::CreateTranslationXY( const Vec2& translationXY )
{		    
	Mat44 temResult;
	temResult.m_values[Tx] = translationXY.x;
	temResult.m_values[Ty] = translationXY.y;
	return temResult;
}		    
		    
const Mat44 Mat44::CreateTranslation3D( const Vec3& translation3D )
{		    
	Mat44 temResult;
	temResult.m_values[Tx] = translation3D.x;
	temResult.m_values[Ty] = translation3D.y;
	temResult.m_values[Tz] = translation3D.z;
	return temResult;
}		    
		    
const Mat44 Mat44::CreateUniformScaleXY( float uniformScaleXY )
{
	Mat44 temResult;
	temResult.m_values[Ix] = uniformScaleXY;
	temResult.m_values[Jy] = uniformScaleXY;
	return temResult;
}

const Mat44 Mat44::CreateNonUniformScaleXY( const Vec2& scaleFactorsXY )
{		    
	Mat44 temResult;
	temResult.m_values[Ix] = scaleFactorsXY.x;
	temResult.m_values[Jy] = scaleFactorsXY.y;
	return temResult;
}		    
		    
const Mat44 Mat44::CreateUniformScale3D( float uniformScale3D )
{		    
	Mat44 temResult;
	temResult.m_values[Ix] = uniformScale3D;
	temResult.m_values[Jy] = uniformScale3D;
	temResult.m_values[Kz] = uniformScale3D;
	return temResult;
}		    
		    
const Mat44 Mat44::CreateNonUniformScale3D( const Vec3& scaleFactorsXYZ )
{
	Mat44 temResult;
	temResult.m_values[Ix] = scaleFactorsXYZ.x;
	temResult.m_values[Jy] = scaleFactorsXYZ.y;
	temResult.m_values[Kz] = scaleFactorsXYZ.z;
	return temResult;
}

void Mat44::TransposeMatrix()
{
	Mat44 temp = *this;
	// I
	m_values[Iy] = temp.m_values[Jx];
	m_values[Iz] = temp.m_values[Kx];
	m_values[Iw] = temp.m_values[Tx];
	// J
	m_values[Jx] = temp.m_values[Iy];
	m_values[Jz] = temp.m_values[Ky];
	m_values[Jw] = temp.m_values[Ty];
	// K
	m_values[Kx] = temp.m_values[Iz];
	m_values[Ky] = temp.m_values[Jz];
	m_values[Kw] = temp.m_values[Tz];
	// T
	m_values[Tx] = temp.m_values[Iw];
	m_values[Ty] = temp.m_values[Jw];
	m_values[Tz] = temp.m_values[Kw];
}

void Mat44::MatrixInvertOrthoNormal()
{
	TransposeMatrix();
}

void Mat44::MatrixInvert()
{
	if( IsMatrixOrthoNormal() ){
		MatrixInvertOrthoNormal();
		return;
	}
	
	MatrixGeneralInvert();
}

bool Mat44::IsMatrixOrthoNormal() const
{
	Mat44 temp = *this;
	temp.TransposeMatrix();
	temp.Multiply( *this );
	if( IsMat44MostlyEqual( temp, IDENTITY ) ){
		return true;
	}
	else{
		return false;
	}
}

Mat44 Mat44::GetTransposeMatrix() const
{
	Mat44 result;
	// I
	result.m_values[Iy] = m_values[Jx];
	result.m_values[Iz] = m_values[Kx];
	result.m_values[Iw] = m_values[Tx];
	// J
	result.m_values[Jx] = m_values[Iy];
	result.m_values[Jz] = m_values[Ky];
	result.m_values[Jw] = m_values[Ty];
	// K
	result.m_values[Kx] = m_values[Iz];
	result.m_values[Ky] = m_values[Jz];
	result.m_values[Kw] = m_values[Tz];
	// T
	result.m_values[Tx] = m_values[Iw];
	result.m_values[Ty] = m_values[Jw];
	result.m_values[Tz] = m_values[Kw];
	
	return result;
}

Mat44 Mat44::GetInvertMatrix() const
{
	if( IsMatrixOrthoNormal() ){
		return GetTransposeMatrix();
	}
	else{
		return GetGeneralInvertMatrix();
	}
}


const Mat44 Mat44::CreateOrthographicProjectionMatrix( const Vec3& min, const Vec3& max )
{
	//TODO
	//ndc.x =  ( x - min.x ) / ( max.x - min.x ) * ( 1.0f - (-1.0f)) + (-1)
	//ndc.x = x / ( max.x - min.x ) - ( min.x / (max.x - min.x )) * 2.0f + (-1.0f);
	// a = 1.0f /(max.x - min.x )
	// b = ( -2.0f * min.x - max.x + min.x) / (max.x - min.x)
	// 
	Vec3 diff = max - min;
	Vec3 sum = max + min;

	float mat[] = {
		2.0f / diff.x,		0.0f,				0.0f,				0.0f,
		0.0f,				2.0f / diff.y,		0.0f,				0.0f,
		0.0f,				0.0f,				1.0f / diff.z,		0.0f,
		-sum.x / diff.x,	-sum.y / diff.y,	-min.z / diff.z,	1.0f
	};
	return Mat44( mat );
}

const Mat44 Mat44::CreatePerspectiveProjectionMatrix( float fovDegrees, float aspectRadio, float nearZ, float farZ )
{
	float fovRadians = ConvertDegreesToRadians( fovDegrees );
	float height = 1.f / (float)tan( fovRadians * 0.5 );
	float zRange = farZ - nearZ;
	float q = 1.0f / zRange;


	Mat44 projection;
	// I
	projection.m_values[Mat44::Ix] = height / aspectRadio;
	projection.m_values[Mat44::Iy] = 0.f;
	projection.m_values[Mat44::Iz] = 0.f;
	projection.m_values[Mat44::Iw] = 0.f;
	// J
	projection.m_values[Mat44::Jx] = 0.f;
	projection.m_values[Mat44::Jy] = height;
	projection.m_values[Mat44::Jz] = 0.f;
	projection.m_values[Mat44::Jw] = 0.f;
	// K
	projection.m_values[Mat44::Kx] = 0.f;
	projection.m_values[Mat44::Ky] = 0.f;
	projection.m_values[Mat44::Kz] = -farZ * q;
	projection.m_values[Mat44::Kw] = -1;
	// T
	projection.m_values[Mat44::Tx] = 0.f;
	projection.m_values[Mat44::Ty] = 0.f;
	projection.m_values[Mat44::Tz] = nearZ * farZ * q;
	projection.m_values[Mat44::Tw] = 0.f;

	return projection;
}

void Mat44::MatrixGeneralInvert()
{
	Mat44 invertMatrix = GetGeneralInvertMatrix();
	for( int i = 0; i < 16; i++ ){
		m_values[i] = invertMatrix.m_values[i];
	}

}

Mat44 Mat44::GetGeneralInvertMatrix() const
{
	float inv[16];
	float det;
	float m[16];
	int i;

	for( i = 0; i < 16; ++i ) {
		m[i] = m_values[i];
	}

	inv[0] = m[5]  * m[10] * m[15] -
		m[5]  * m[11] * m[14] -
		m[9]  * m[6]  * m[15] +
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] -
		m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] +
		m[4]  * m[11] * m[14] +
		m[8]  * m[6]  * m[15] -
		m[8]  * m[7]  * m[14] -
		m[12] * m[6]  * m[11] +
		m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9]  * m[15] -
		m[4]  * m[11] * m[13] -
		m[8]  * m[5]  * m[15] +
		m[8]  * m[7]  * m[13] +
		m[12] * m[5]  * m[11] -
		m[12] * m[7]  * m[9];

	inv[12] = -m[4]  * m[9]  * m[14] +
		m[4]  * m[10] * m[13] +
		m[8]  * m[5]  * m[14] -
		m[8]  * m[6]  * m[13] -
		m[12] * m[5]  * m[10]  +
		m[12] * m[6]  * m[9];

	inv[1] = -m[1]  * m[10] * m[15] +
		m[1]  * m[11] * m[14] +
		m[9]  * m[2]  * m[15] -
		m[9]  * m[3]  * m[14] -
		m[13] * m[2]  * m[11] +
		m[13] * m[3]  * m[10];

	inv[5] = m[0]  * m[10] * m[15] -
		m[0]  * m[11] * m[14] -
		m[8]  * m[2]  * m[15] +
		m[8]  * m[3]  * m[14] +
		m[12] * m[2]  * m[11] -
		m[12] * m[3]  * m[10];

	inv[9] = -m[0]  * m[9]  * m[15] +
		m[0]  * m[11] * m[13] +
		m[8]  * m[1]  * m[15] -
		m[8]  * m[3]  * m[13] -
		m[12] * m[1]  * m[11] +
		m[12] * m[3]  * m[9];

	inv[13] = m[0]  * m[9]  * m[14] -
		m[0]  * m[10] * m[13] -
		m[8]  * m[1]  * m[14] +
		m[8]  * m[2]  * m[13] +
		m[12] * m[1]  * m[10] -
		m[12] * m[2]  * m[9];

	inv[2] = m[1]  * m[6] * m[15] -
		m[1]  * m[7] * m[14] -
		m[5]  * m[2] * m[15] +
		m[5]  * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] +
		m[0]  * m[7] * m[14] +
		m[4]  * m[2] * m[15] -
		m[4]  * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] -
		m[0]  * m[7] * m[13] -
		m[4]  * m[1] * m[15] +
		m[4]  * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] +
		m[0]  * m[6] * m[13] +
		m[4]  * m[1] * m[14] -
		m[4]  * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	det = 1.f / det;

	Mat44 ret;
	for( i = 0; i < 16; i++ ) {
		ret.m_values[i] = (float)( inv[i] * det );
	}
	return ret;
}
