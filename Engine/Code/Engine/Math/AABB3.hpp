#pragma  once
#include "Engine/Math/Vec3.hpp"

struct AABB3 
{
public:
	AABB3(){}
	AABB3( Vec3 minimum, Vec3 maximum );

public:
	// Accessor
	bool IsCube() const;
	float GetDiagonalsLength() const;
	float GetSideLength() const;
	Vec3 GetCenter() const;
public:
	Vec3 min;
	Vec3 max;
};