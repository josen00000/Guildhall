#pragma once
#include "Engine/Math/Vec3.hpp"

struct Cone3
{
	Vec3	m_start;
	Vec3	m_end;
	float	m_radius;
	
public:
	Cone3() = default;
	~Cone3() = default;
	explicit Cone3( const Vec3& start, const Vec3& end, float radius );
	explicit Cone3( const Vec3& start, const Vec3& direction, float length, float radius );

public:
	//Accessor
	Vec3		GetStartPos() const { return m_start; }
	Vec3		GetEndPos() const { return m_end; }
	float		GetRadius() const { return m_radius; }
	float		GetLength() const;

	//Mutator
	void	SetStartPos( const Vec3& start );
	void	SetEndPos( const Vec3& end );
	void	SetRadius( float radius );
};

