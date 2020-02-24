#pragma once

class PhysicsMaterial {
public:
	PhysicsMaterial(){}
	PhysicsMaterial( float res );
	~PhysicsMaterial(){}

	static float GetRestitutionBetweenTwoMaterial( PhysicsMaterial PM1, PhysicsMaterial PM2 );
public:
	float restitution = 1.f;
};