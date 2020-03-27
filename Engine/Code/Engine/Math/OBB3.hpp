#pragma  once
#include "Engine/Math/Vec3.hpp"

struct OBB3 
{
public:
	OBB3( Vec3 center, Vec3 halfDimension, Vec3 right, Vec3 up );

public:
	// Accessor
	bool IsCube() const;
	float GetDiagonalsLength() const;
	float GetSideLength() const;
	//Vec3 GetLeft

public:
	Vec3 m_center;
	Vec3 m_halfDimension;
	Vec3 m_right;
	Vec3 m_up;
};