#include "OBB3.hpp"

OBB3::OBB3( Vec3 center, Vec3 halfDimension, Vec3 right, Vec3 up )
	:m_center(center)
	,m_up(up)
	,m_halfDimension(halfDimension)
	,m_right(right)
{
}
