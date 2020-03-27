#include "AABB3.hpp"

AABB3::AABB3( Vec3 minimum, Vec3 maximum )
	:min(minimum)
	,max(maximum)
{
}

bool AABB3::IsCube() const
{
	if( max.x - min.x == max.y - min.y && max.x - min.x == max.z - min.z ){
		return true;
	}
	else{
		return false;
	}
}

float AABB3::GetDiagonalsLength() const
{
	Vec3 disp = max - min;
	return disp.GetLength();
}

float AABB3::GetSideLength() const
{
	return max.x - min.x;
}

Vec3 AABB3::GetCenter() const
{
	return ( max + min ) / 2;
}
