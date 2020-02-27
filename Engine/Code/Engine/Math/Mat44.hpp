#pragma once

struct Vec2;
struct Vec3;
struct Vec4;


//////////////////////////////////////////////////////////////////////////
///[ Ix, Jx, Kx, Tx][x]   
///[ Iy, Jy, Ky, Ty][y]
///[ Iz, Jz, Kz, Tz][z]
///[ Iw, Jw, Kw, Tw][w]
/// Rotate X
///[ 1, 0,   0,    0][x] 
///[ 0, cos, -sin, 0][y]
///[ 0,	sin, cos,  0][z]	
///[ 0, 0,   0,    1][w]
///[

struct Mat44
{
public:
	float m_values[16];
	enum {Ix, Iy, Iz, Iw,  Jx, Jy, Jz, Jw,  Kx, Ky, Kz, Kw, Tx, Ty, Tz, Tw};
	static Mat44 IDENTITY;

public:
	//Construction
	Mat44();
	explicit Mat44( float* sixteenValuesBasisMajor );
	explicit Mat44( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D );
	explicit Mat44( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D );
	explicit Mat44( const Vec4& iBasisHomogeneous, const Vec4& jBasisHomogeneous, const Vec4& kBasisHomogeneous, const Vec4& translationHomogeneous );


	// Transforming positions & vector quantities using this matrix;
	const Vec2 TransformVector2D( const Vec2& vectorQuantitiy) const;
	const Vec3 TransformVector3D( const Vec3& vectorQuantitiy) const;
	const Vec2 TransformPosition2D( const Vec2& position) const;
	const Vec3 TransformPosition3D( const Vec3& position) const;
	const Vec4 TransformHomogeneousPoint3D( const Vec4& homogeneousPoint) const;

	//basic accessors
	const float*		GetAsFloatArray() const			{return &m_values[Ix];}
	float*				GetAsFloatArray()				{return &m_values[Ix];}
	const Vec2			GetIBasis2D() const;
	const Vec2			GetJBasis2D() const;
	const Vec2			GetTranslation2D() const;
	const Vec3			GetIBasis3D() const;
	const Vec3			GetJBasis3D() const;
	const Vec3			GetKBasis3D() const;
	const Vec3			GetTranslation3D() const;
	const Vec4			GetIBasis4D() const;
	const Vec4			GetJBasis4D() const;
	const Vec4			GetKBasis4D() const;
	const Vec4			GetTranslation4D() const;

	//Basic mutators
	void SetTranslation2D( const Vec2& translation2D);
	void SetTranslation3D( const Vec3& translation3D);
	void SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D);
	void SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D);
	void SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D);
	void SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D);
	void SetBasisVectors4D( const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D);

	//Transformation mutators; append (concatenate) a new transform to this matrix
	void RotateXDegrees(float degreesAboutX);
	void RotateYDegrees(float degreesAboutY);
	void RotateZDegrees(float degreesAboutZ);
	void Translate2D( const Vec2& translationXY);
	void Translate3D( const Vec3& translation3D);
	void ScaleUniform2D( float uniformScaleXY);
	void ScaleNonUniform2D( const Vec2& ScaleFactorsXY);
	void ScaleUniform3D( float uniformScaleXYZ);
	void ScaleNonUniform3D( const Vec3& scaleFactorsXYZ);
	void Multiply(const Mat44& arbitraryTransFormationToAppend);
	
	// Static creation methods to create a matrix of a certain transformation type
	static const Mat44		CreateXRotationDegrees( float degreesAboutX);
	static const Mat44		CreateYRotationDegrees( float degreesAboutY);
	static const Mat44		CreateZRotationDegrees( float degreesAboutZ);
	static const Mat44		CreateTranslationXY( const Vec2& translationXY);
	static const Mat44		CreateTranslation3D( const Vec3& translation3D);
	static const Mat44		CreateUniformScaleXY( float uniformScaleXY);
	static const Mat44		CreateNonUniformScaleXY( const Vec2& scaleFactorsXY);
	static const Mat44		CreateUniformScale3D( float uniformScale3D);
	static const Mat44		CreateNonUniformScale3D( const Vec3& scaleFactorsXYZ);

	// Transpose and Invert
	void TransposeMatrix();
	void MatrixInvert();
	bool IsMatrixOrthoNormal() const;
	Mat44 GetTransposeMatrix() const;
	Mat44 GetInvertMatrix() const;

	// Projection
	static const Mat44 CreateOrthographicProjectionMatrix( const Vec3& min, const Vec3& max );
	static const Mat44 CreatePersectiveProjectionMatrix( float fovDegrees, float aspectRadio, float nearZ, float farZ );

private:
	// Transpose and Invert
	void MatrixInvertOrthoNormal();
	void MatrixGeneralInvert();
	Mat44 GetGeneralInvertMatrix() const;
	const Mat44 operator*( const Mat44& rhs ) const = delete;
};

