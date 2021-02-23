#pragma once
#include "Engine/Math/Vec3.hpp"

struct Vec2;

struct Vec4
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

	Vec4(){}
	~Vec4(){}
	Vec4( Vec3 XYZ, float w= 0 );
	explicit Vec4(float inputX, float inputY, float inputZ, float inputW  );
	Vec4( Vec2 XY );

	// Accessor
	Vec3 getVec3() const { return Vec3( x, y, z ); }

	// Mutators
	void SetFormText(const char* text);
	void SetXYZ( Vec3 color ); // only use for color
	void SetW( float intensity ); // only use for intensity

	const Vec4 operator-( const Vec4& vecToSubtract ) const;
	const Vec4 operator+( const Vec4& vecToAdd ) const;
	const Vec4 operator*=( float uniformScale ) const;

	// static variable
	static Vec4 ZERO;
	static Vec4 ONE;

};

