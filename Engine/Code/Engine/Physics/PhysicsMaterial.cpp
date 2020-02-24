#include "PhysicsMaterial.hpp"


PhysicsMaterial::PhysicsMaterial( float res )
	:restitution(res)
{
}

float PhysicsMaterial::GetRestitutionBetweenTwoMaterial( PhysicsMaterial PM1, PhysicsMaterial PM2 )
{
	return ( PM1.restitution + PM2.restitution ) / 2;
}
