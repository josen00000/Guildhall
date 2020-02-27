#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Transform.hpp"
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

typedef uint CameraClearFlags;

class Camera{
	friend class RenderContext;
public:
	Camera(){}
	~Camera();
	Camera( const Camera& camera ) = delete;
	explicit Camera( const Vec2& bottomLeft, const Vec2& topRight );

	void Translate( const Vec3& translation );

public:
	// Accessor
	float	GetCameraHeight() const;
	float	GetCameraWidth() const;
	Vec2	GetOrthoBottomLeft() const;
	Vec2	GetOrthoTopRight() const;
	Vec3	GetPosition() const;
	AABB2	GetCameraAsBox() const;

	Mat44	GetViewMatrix() const { return m_view; }
	Mat44	GetProjectionMatrix() const { return m_projection; }

	bool	GetShouldClearColor() const { return m_shouldClearColor; }
	Rgba8	GetClearColor() const { return m_clearColor; }

	Texture*		GetColorTarget() const { return m_colorTarget; }
	Texture*		GetDepthStencilTarget() const { return m_depthStencilTarget; }
	RenderBuffer*	GetOrCreateCameraBuffer( RenderContext* ctx );
	
	// Mutator
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	void SetPosition( const Vec3& position );
	void SetShouldClearColor( bool shouldClearColor );
	void SetClearMode( unsigned int clearFlags, Rgba8 color, float depth = 0.0f , unsigned int stencil = 0 );
	void SetColorTarget( Texture* colorTarget );
	void SetDepthStencilTarget( Texture* texture );
	void SetPitchRollYawRotation( float pitch, float roll, float yaw );

	//void SetProjectionOrthographic( float size, float nearZ, float farZ );
	void SetProjectionPerspective( float fov, float nearZ, float farZ );
	void UpdateCameraUBO();

	// Helper
	Vec3 ClientTOWorld( Vec2 client, float ndcZ );
	Vec3 WorldToClient( Vec3 worldPos );

	void UpdateModelMatrixFromTranslation();
	void UpdateViewMatrix();
	void UpdateModelMatrix();

private:

public:
	bool m_shouldClearColor = true;
	uint m_clearMode = 0;
	Vec2 m_outputSize;
	Vec3 m_position = Vec3::ZERO;
	Rgba8 m_clearColor = Rgba8::BLACK;
	//Transform m_transform;
	float m_clearDepth = 1.f;
	float m_clearStencil = 0.f;
	Transform m_transform;

private:
	Texture* m_colorTarget = nullptr;    // texture to render to 
	Texture* m_depthStencilTarget = nullptr;
	RenderBuffer* m_cameraUBO = nullptr; // render data( vertices )
	Mat44 m_projection;
	Mat44 m_view;
	Mat44 m_model;
};
