#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/OBB2.hpp"

struct Capsule2
{
	Vec2	m_start;
	Vec2	m_end;
	float	m_radius;
	
public:
	Capsule2() = default;
	~Capsule2() = default;
	explicit Capsule2( const Vec2& start, const Vec2& end, float radius );
	explicit Capsule2( const Vec2& start, const Vec2& diretion, float length, float radius );

public:
	//Accessor
	Vec2		GetStartPos() const;
	Vec2		GetEndPos() const;
	float		GetRadius() const;
	float		GetLength() const;
	Vec2		GetNormalizedDirection() const;
	float		GetlengthSquared() const;
	Vec2		GetNearestPoint( const Vec2& refPos ) const;
	bool		IsPointInSide( const Vec2& refPos ) const;
	const OBB2	GetCapsuleOBB2() const;

	//Mutator
	void	SetStartPos( const Vec2& start );
	void	SetEndPos( const Vec2& end );
	void	SetRadius( float radius );
	void	SetLength( float length );

};

