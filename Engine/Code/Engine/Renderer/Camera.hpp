#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Texture.hpp"

class RenderBuffer;


enum eCameraClearBitFlag : unsigned int {
	// what is this
	CLEAR_COLOR_BIT		= (1 << 0),
	CLEAR_DEPTH_BIT		= (1 << 1),
	CLEAR_STENCIL_BIT	= (1 << 2),

};


class Camera{
	friend class RenderContext;
public:
	Camera(){}
	~Camera();
	Camera( const Camera& camera )=delete;
	explicit Camera( const Vec2& bottomLeft, const Vec2& topRight );

	void Translate( const Vec3& translation );
public:
	// Accessor
	float	GetCameraHeight() const;
	float	GetCameraWidth() const;
	float	GetAspectRatio() const;
	Vec2	GetOrthoBottomLeft() const;
	Vec2	GetOrthoTopRight() const;
	Vec3	GetPosition() const;
	AABB2	GetCameraBox() const;

	bool	GetShouldClearColor() const { return m_shouldClearColor; }
	Rgba8	GetClearColor() const { return m_clearColor; }

	Texture* GetColorTarget() const { return m_colorTarget; }
	RenderBuffer* GetOrCreateCameraBuffer( RenderContext* ctx );

	// Mutator
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	void SetOutputSize( Vec2 size );
	void SetOutputSize( float aspectRatio, float height );
	void SetPosition( const Vec3& position );
	void SetProjectionOrthographic( float height, float nearZ = -1.0f, float farZ = 1.0f );
	
	void SetShouldClearColor( bool shouldClearColor );
	void SetClearMode( unsigned int clearFlags, Rgba8 color, float depth = 0.0f , unsigned int stencil = 0 );
	void SetColorTarget( Texture* colorTarget );

	void UpdateCameraUBO();

	Vec2 ClientToWorldPosition( Vec2 clientPos );

private:
	Vec2 GetOrthoMin() const;
	Vec2 GetOrthoMax() const;

public:
	uint m_clearMode = 0;
	bool m_shouldClearColor = true;
	Vec2 m_outputSize;
	Vec3 m_position = Vec3::ZERO;
	Rgba8 m_clearColor = Rgba8::BLACK;

private:
	Texture* m_colorTarget;    // texture to render to 
	RenderBuffer* m_cameraUBO = nullptr; // render data( vertices )
	Mat44 m_projection;
	Mat44 m_view;
};
