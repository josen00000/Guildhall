#pragma once

class PhysicsMaterial {
public:
	PhysicsMaterial(){}
	PhysicsMaterial( float res );
	~PhysicsMaterial(){}

	static float GetRestitutionBetweenTwoMaterial( PhysicsMaterial PM1, PhysicsMaterial PM2 );
	void UpdateRestitution( float deltaRes );
	void UpdateFriction( float deltaFric );

public:
	float m_restitution = 0.5f;
	float m_friction	= 0.f;
};