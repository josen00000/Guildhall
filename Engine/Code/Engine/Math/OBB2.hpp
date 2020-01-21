#pragma once
#include <vector>
#include<Engine/Math/vec2.hpp> 

struct AABB2;
struct Vertex_PCU;
struct Rgba8;

struct OBB2 {
public:
	Vec2 m_iBasisNormal = Vec2( 1.f, 0.f );
	Vec2 m_halfDimensions;
	Vec2 m_center;


public:
	// Construction
	OBB2()=default;
	~OBB2()=default;
	OBB2( const OBB2& copyFrom )= default;
	explicit OBB2( const Vec2& fullDimensions, const Vec2& center, const Vec2& iBasisNormal = Vec2(1.f, 0.f) );
	explicit OBB2( const Vec2& fullDimensions, const Vec2& center, float orientationDegrees );
	explicit OBB2( const AABB2& asAxisAlignedBox, float orientationDegrees );

	//Accessors(const methods)
	bool		IsPointInside( const Vec2& point ) const;
	const Vec2	GetCenter() const;
	const Vec2	GetDimensions() const;
	const Vec2	GetIBasisNormal() const;
	const Vec2	GetJBasisNormalized() const;
	const Vec2	GetLeftDownCornerForIBasis() const;
	const Vec2	GetRightDownCornerForIBasis() const;
	const Vec2	GetLeftUpCornerForIBasis() const;
	const Vec2	GetrightUpCornerForIBasis() const;
	const Vec2	GetNearestPoint( const Vec2& referencePos ) const;
	const Vec2	GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const;
	const Vec2	GetUVForPoint( const Vec2& point ) const;
	float		GetOrientationDegrees() const;
	float		GetOuterRadius() const;
	float		GetInnerRadius() const;
	void		GetCornerPositions( Vec2* out_fourPoints ) const;
	

	// Nutators
	void RotateDegrees( float deltaDegrees);
	void SetOrientationDegrees( float orientationDegrees);
	void Translate( const Vec2& translation);
	void SetCenter( const Vec2& newCenter);
	void SetDimensions(const Vec2& newDimensions);
	void StretchToIncludePoint(const Vec2& point);
	void Fix();
	

	// help functions
	Vec2 TransformPointIntoOBBCoords( const Vec2& point ) const;
	Vec2 TransformPointIntoWorldCoords( const Vec2& pointInOBB ) const;
	static void AddVertsForOBB2D( std::vector<Vertex_PCU>& vertices, const OBB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs );

	//Operators
	bool operator==(const OBB2& compareWith) const;
	//void operator=(const OBB2& assignFrom)= default;
};