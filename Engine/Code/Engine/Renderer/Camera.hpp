#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Renderer/Texture.hpp"

enum eCameraClearBitFlag : unsigned int {
	CLEAR_COLOR_BIT		= (1 << 0),
	CLEAR_DEPTH_BIT		= (1 << 1),
	CLEAR_STENCIL_BIT	= (1 << 2),

};

class Camera{

public:
	Camera(){}
	~Camera(){}
	Camera(const Camera& camera);
	explicit Camera(const Vec2& bottomLeft, const Vec2& topRight);

public:
	// Accessor
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	Vec2 GetPosition() const;
	bool GetShouldClearColor() const { return m_shouldClearColor; }
	Rgba8 GetClearColor() const { return m_clearColor; }
	Texture* GetColorTarget() const { return m_colorTarget; }
	
	// Mutator
	void SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight);
	void SetPosition(const Vec2 inPosition);
	void SetShouldClearColor( bool shouldClearColor );
	void SetClearMode( unsigned int clearFlags, Rgba8 color, float depth = 0.0f , unsigned int stencil = 0 );
	void SetColorTarget( Texture* colorTarget );
	
	void UpdateCamera();


public:
	AABB2 m_AABB2 = AABB2(Vec2(0,0),Vec2(0,0));
	Vec2 m_position = Vec2(0.f,0.f);
	float m_width = 0.f;
	float m_height = 0.f;

	unsigned int m_clearMode = 0;
	Rgba8 m_clearColor = Rgba8::BLACK;

	Texture* m_colorTarget;
private:
	bool m_shouldClearColor = true;
	
};
