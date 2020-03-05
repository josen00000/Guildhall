#include "PhysicsMaterial.hpp"
#include "Engine/Math/MathUtils.hpp"

PhysicsMaterial::PhysicsMaterial( float res )
	:m_restitution(res)
{
}

float PhysicsMaterial::GetRestitutionBetweenTwoMaterial( PhysicsMaterial PM1, PhysicsMaterial PM2 )
{
	return ( PM1.m_restitution + PM2.m_restitution ) / 2;
}

void PhysicsMaterial::UpdateRestitution( float deltaRes )
{
	m_restitution += deltaRes;
	m_restitution = ClampFloat( 0.f, 1.f, m_restitution );
}

void PhysicsMaterial::UpdateFriction( float deltaFric )
{
	m_friction += deltaFric;
	m_friction = ClampFloat( 0.f, 1.f, m_friction );
}
