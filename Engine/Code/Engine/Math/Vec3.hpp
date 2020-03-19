#pragma once
#include<Engine/Math/MathUtils.hpp>

struct Vec3 {
public:
	float x=0.f;
	float y=0.f;
	float z=0.f;

public:
	//Construction

	Vec3(){}
	~Vec3(){}
	Vec3(const Vec3&copyFrom);
	explicit Vec3( float initialValue );
	explicit Vec3(float initialX,float initialY,float initialZ);
	Vec3(const Vec2& copyFromVec2,float initialZ=0 );
	// Accessors(const methods)
	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquared() const;
	float GetLengthXYSquared() const;
	float GetThetaDegrees() const;
	float GetPhiDegrees() const;
	float GetAngleAboutZRadians() const;
	float GetAngleAboutZDegrees() const;
	const Vec3 GetRotatedAboutZRadians( float deltaRadians ) const;
	const Vec3 GetRotatedAboutZDegrees( float deltaDegrees ) const;
	const Vec3 GetClamped( float maxLength )const;
	const Vec3 GetNormalized() const;

	void SetText( const char* text);

	static const Vec3 MakeFromPolarDegrees( float theta, float phi, float length=1.f );

public:
	// Operators (const)
	bool		operator==( const Vec3& compare ) const;		// Vec3 == Vec3
	bool		operator!=( const Vec3& compare ) const;		// Vec3 != Vec3
	const Vec3	operator+( const Vec3& vecToAdd ) const;		// Vec3 + Vec3
	const Vec3	operator-( const Vec3& vecToSubtract ) const;	// Vec3 - Vec3
	const Vec3	operator-() const;								// -Vec3, i.e. "unary negation"
	const Vec3	operator*( float uniformScale ) const;			// Vec3 * float
	const Vec3	operator*( const Vec3& vecToMultiply ) const;	// Vec3 * Vec3
	const Vec3	operator/( float inverseScale ) const;			// Vec3 / float

																// Operators (self-mutating / non-const)
	void		operator+=( const Vec3& vecToAdd );				// Vec3 += Vec3
	void		operator-=( const Vec3& vecToSubtract );		// Vec3 -= Vec3
	void		operator*=( const float uniformScale );			// Vec3 *= float
	void		operator/=( const float uniformDivisor );		// Vec3 /= float
	void		operator=( const Vec3& copyFrom );				// Vec3 = Vec3

																// Standalone "friend" functions that are conceptually, but not actually, part of Vec3::
	friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * Vec3
	
	// static variable
	static Vec3 ZERO;
	static Vec3 ONE;
	static Vec3 CENTER;
};