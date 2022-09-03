#pragma once
#include "Engine/Math/Vec2.hpp"

struct IntVec2;

struct AABB2 {
	Vec2 min;
	Vec2 max;

public:
	AABB2() = default;
	~AABB2() = default;
	explicit AABB2( const Vec2& i_min, const Vec2& i_max );
	explicit AABB2( const IntVec2& i_min, const IntVec2& i_max );
	explicit AABB2( float minX, float minY, float maxX, float maxY );
	explicit AABB2( Vec2 const& center, float width, float height );

	//Accessors(const methods)
	bool		IsPointInside( const Vec2& point ) const;
	bool		IsPointMostInEdge( const Vec2& point, float epsilon = 0.01f ) const;
	const Vec2	GetCenter() const;
	const Vec2	GetDimensions() const;
	const Vec2	GetNearestPoint( const Vec2& refPos ) const;
	const Vec2	GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const;
	const Vec2	GetUVForPoint( const Vec2& point ) const;
	AABB2		GetBoxWithin( const Vec2& dimensions, const Vec2& alignment ) const;
	AABB2		GetBoxAtLeft( float fractionOfWidth, float additionalWidth=0.f ) const;
	AABB2		GetBoxAtRight( float fractionOfWidth, float additionalWidth=0.f ) const;
	AABB2		GetBoxAtBottom( float fractionOfHeight, float additionalWidth=0.f ) const;
	AABB2		GetBoxAtTop( float fractionOfHeight, float additionalWidth=0.f ) const;
	float		GetOuterRadius() const;
	float		GetInnerRadius() const;
	float		GetWidth() const;
	float		GetHeight() const;
	float		GetDiagonalLength();
	float		GetArea() const;
	void		GetCornerPositions( Vec2* out_fourPoints ) const;

	//Operators
	bool operator==( const AABB2& compareWith ) const;
	void operator=( const AABB2& assignFrom );
};