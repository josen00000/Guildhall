#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"

class Camera{

public:
	Camera(){}
	~Camera(){}
	Camera( const Camera& camera );
	explicit Camera( const Vec2& bottomLeft, const Vec2& topRight );

public:
	// Accessor
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	Vec2 GetPosition() const;
	float GetHeight() const;
	float GetWidth() const;
	float GetAspectRatio() const;

	// Mutator
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	void SetOutputSize( Vec2 size );
	void SetOutputSize( float aspectRatio, float height );
	void SetPosition( const Vec3& position );
	void SetProjectionOrthographic( float height, float nearZ = -1.0f, float farZ = 1.0f );
	
	Vec2 ClientToWorldPosition( Vec2 clientPos );

private:
	Vec2 GetOrthoMin() const;
	Vec2 GetOrthoMax() const;

public:
	Vec2 m_outputSize;
	Vec3 m_position = Vec3( 0.f, 0.f, 0.f );
};
