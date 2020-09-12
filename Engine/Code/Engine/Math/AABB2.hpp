#pragma once
#include <vector>
#include "Engine/Math/vec2.hpp" 
// #include "Engine/Core/Rgba8.hpp"

struct Vertex_PCU;
struct Rgba8;

struct AABB2 {
public:
	Vec2 mins;
	Vec2 maxs;

	static const AABB2 ZERO_TO_ONE;

public:
	// Construction
	AABB2()=default;
	~AABB2()=default;
	AABB2(const AABB2& copyFrom);
	explicit AABB2(const Vec2& mins, const Vec2& maxs);
	explicit AABB2(const IntVec2& intMins, const IntVec2& intMaxs);
	explicit AABB2(float minX,float minY, float maxX,float maxY);

	//Accessors(const methods)
	bool IsPointInside(const Vec2& point) const;
	const Vec2	GetCenter() const;
	const Vec2	GetDimensions() const;
	const Vec2	GetNearestPoint(const Vec2& referencePosition) const;
	const Vec2	GetPointAtUV(const Vec2& uvCoordsZeroToOne) const;
	const Vec2	GetUVForPoint(const Vec2& point) const;
	AABB2		GetBoxWithin( const Vec2& dimensions, const Vec2& alignment ) const;
	AABB2		GetBoxAtLeft( float fractionOfWidth, float additionalWidth=0.f ) const;
	AABB2		GetBoxAtRight( float fractionOfWidth, float additionalWidth=0.f ) const;
	AABB2		GetBoxAtBottom( float fractionOfHeight, float additionalWidth=0.f ) const;
	AABB2		GetBoxAtTop( float fractionOfHeight, float additionalWidth=0.f ) const;
	float		GetOuterRadius() const;
	float		GetInnerRadius() const;
	void		GetCornerPositions( Vec2* out_fourPoints ) const;

	// Mutators
	void		Translate(const Vec2& translation);
	void		SetCenter(const Vec2& newCenter);
	void		SetDimensions(const Vec2& newDimensions);
	void		StretchToIncludePoint(const Vec2& point);
	void		MoveToIncludePoint( const Vec2& point );
	void		filledWithinAABB2(const AABB2 filledAABB2);
	AABB2		CarveBoxOffLeft( float fractionOfWidth, float additionalWidhth=0.f );
	AABB2		CarveBoxOffRight( float fractionOfWidth, float additionalWidhth=0.f );
	AABB2		CarveBoxOffBottom( float fractionOfHeight, float additionalWidhth=0.f );
	AABB2		CarveBoxOffTop( float fractionOfHeight, float additionalWidhth=0.f );

	// Help functions
	static void AddVertsForAABB2D(std::vector<Vertex_PCU>& vertices, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs);

	//Operators
	bool operator==(const AABB2& compareWith) const;
	void operator=(const AABB2& assignFrom);
};