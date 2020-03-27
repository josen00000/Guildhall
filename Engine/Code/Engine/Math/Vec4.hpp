#pragma once

struct Vec3;

struct Vec4
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

	Vec4(){}
	~Vec4(){}
	Vec4( Vec3 XYZ, float w );
	Vec4(float inputX, float inputY, float inputZ, float inputW  );

	//Mutators
	void SetFormText(const char* text);

	const Vec4 operator-( const Vec4& vecToSubtract ) const;
	const Vec4 operator*=( float uniformScale ) const;
};

