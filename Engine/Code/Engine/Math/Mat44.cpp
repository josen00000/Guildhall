#include "Mat44.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"






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

	TransformBy(temRotate);
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

	TransformBy( temRotate );

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

	TransformBy( temRotate );

}

void Mat44::Translate2D( const Vec2& translationXY )
{
	Mat44 temTranslate;
	temTranslate.m_values[Tx] = translationXY.x;
	temTranslate.m_values[Ty] = translationXY.y;

	TransformBy(temTranslate);
}

void Mat44::Translate3D( const Vec3& translation3D )
{
	Mat44 temTranslate;
	temTranslate.m_values[Tx] = translation3D.x;
	temTranslate.m_values[Ty] = translation3D.y;
	temTranslate.m_values[Tz] = translation3D.z;

	TransformBy( temTranslate );
}

void Mat44::ScaleUniform2D( float uniformScaleXY )
{
	Mat44 temScale;
	temScale.m_values[Ix] = uniformScaleXY;
	temScale.m_values[Jy] = uniformScaleXY;

	TransformBy(temScale);
}

void Mat44::ScaleNonUniform2D( const Vec2& ScaleFactorsXY )
{
	Mat44 temScale;
	temScale.m_values[Ix] = ScaleFactorsXY.x;
	temScale.m_values[Jy] = ScaleFactorsXY.y;

	TransformBy( temScale );
}

void Mat44::ScaleUniform3D( float uniformScaleXYZ )
{
	Mat44 temScale;
	temScale.m_values[Ix] = uniformScaleXYZ;
	temScale.m_values[Jy] = uniformScaleXYZ;
	temScale.m_values[Kz] = uniformScaleXYZ;

	TransformBy( temScale );

}

void Mat44::ScaleNonUniform3D( const Vec3& scaleFactorsXYZ )
{
	Mat44 temScale;
	temScale.m_values[Ix] = scaleFactorsXYZ.x;
	temScale.m_values[Jy] = scaleFactorsXYZ.y;
	temScale.m_values[Kz] = scaleFactorsXYZ.z;

	TransformBy( temScale );

}

void Mat44::TransformBy( const Mat44& arbitraryTransFormationToAppend )
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