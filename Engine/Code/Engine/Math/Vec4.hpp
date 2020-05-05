#pragma once

struct Vec3;
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

	// Mutators
	void SetFormText(const char* text);
	void SetXYZ( Vec3 color ); // only use for color
	void SetW( float intensity ); // only use for intensity

	const Vec4 operator-( const Vec4& vecToSubtract ) const;
	const Vec4 operator*=( float uniformScale ) const;

	// static variable
	static Vec4 ZERO;
	static Vec4 ONE;

};

