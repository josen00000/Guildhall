#pragma once
#include "Engine/Math/Vec3.hpp"

struct Cylinder3
{
	Vec3	m_start;
	Vec3	m_end;
	float	m_radius;
	
public:
	Cylinder3() = default;
	~Cylinder3() = default;
	explicit Cylinder3( const Vec3& start, const Vec3& end, float radius );
	explicit Cylinder3( const Vec3& start, const Vec3& direction, float length, float radius );

public:
	//Accessor
	Vec3		GetStartPos() const { return m_start; }
	Vec3		GetEndPos() const { return m_end; }
	float		GetRadius() const { return m_radius; }
	float		GetLength() const;
	float		GetlengthSquared() const;

	//Mutator
	void	SetStartPos( const Vec3& start );
	void	SetEndPos( const Vec3& end );
	void	SetRadius( float radius );
};

