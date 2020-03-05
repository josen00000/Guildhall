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

enum ProjectionType
{
	PROJECTION_ORTHOGRAPHIC,
	PROJECTION_PERSPECTIVE
};

typedef uint CameraClearFlags;

class Camera{
	friend class RenderContext;
public:
	Camera(){}
	~Camera();

	Camera( const Camera& camera ) = delete;
	explicit Camera( const Vec2& bottomLeft=Vec2::ZERO, const Vec2& topRight=Vec2::ONE );
	explicit Camera( float fov=60.f, float nearZ=-0.1, float farZ=-100, Vec3 pos=Vec3::ZERO, Vec3 rotPRY=Vec3::ZERO, Vec3 scale=Vec3::ONE );
	static Camera* CreateOrthographicCamera( const Vec2& bottomLeft, const Vec2& topRight );
	static Camera* CreatePerspectiveCamera( float fov, float nearZ, float farZ, Vec3 pos=Vec3::ZERO, Vec3 rotPRY=Vec3::ZERO, Vec3 scale=Vec3::ONE );
	void Translate( const Vec3& translation );

public:
	// Accessor
	float	GetCameraHeight() const;
	float	GetCameraWidth() const;
	float	GetOutputAspectRatio();
	Vec2	GetOrthoBottomLeft() const;
	Vec2	GetOrthoTopRight() const;
	Vec3	GetPosition() const;

	AABB2	GetCameraAsBox() const;
	ProjectionType GetCameraProjectionType() const{ return m_projectionType; }

	Mat44	GetViewMatrix() const { return m_view; }
	Mat44	GetProjectionMatrix() const { return m_projection; }
	Mat44	GetModelMatrix() const;


	bool	GetShouldClearColor() const { return m_shouldClearColor; }
	Rgba8	GetClearColor() const { return m_clearColor; }


	Texture*		GetColorTarget() const;
	Texture*		GetDepthStencilTarget() const { return m_depthStencilTarget; }
	Texture*		GetOrCreateDepthStencilTarget( RenderContext* ctx );
	RenderBuffer*	GetOrCreateCameraBuffer( RenderContext* ctx );

		// Mutator
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	void SetPosition( const Vec3& position );
	void SetProjectionOrthographic( float height, float nearZ = -1.0f, float farZ = 1.0f );
	
	void SetShouldClearColor( bool shouldClearColor );
	void SetClearMode( unsigned int clearFlags, Rgba8 color, float depth = 0.0f , unsigned int stencil = 0 );
	void SetColorTarget( Texture* colorTarget );
	void SetDepthStencilTarget( Texture* texture );
	void SetPitchRollYawRotation( float pitch, float roll, float yaw );

	void SetProjectionPerspective( float fov=60, float nearZ=-0.1, float farZ=-100 );

	void UpdateCameraUBO();
	void UpdateCameraRotation( Vec3 deltaRot );

	// Helper
	Vec3 ClientToWorld( Vec2 client, float ndcZ ) const;
	Vec3 WorldToClient( Vec3 worldPos );

	void UpdateViewMatrix();

private:
	Vec2 GetOrthoMin() const;
	Vec2 GetOrthoMax() const;

public:
	uint m_clearMode = 0;
	bool m_shouldClearColor = true;
	Vec2 m_dimension;
	Rgba8 m_clearColor = Rgba8::BLACK;
	float m_clearDepth = 1.f;
	uint m_clearStencil = 0;
	Transform m_transform;
	ProjectionType m_projectionType = PROJECTION_ORTHOGRAPHIC;

private:
	Texture* m_colorTarget = nullptr;    // texture to render to 
	Texture* m_depthStencilTarget = nullptr;
	RenderBuffer* m_cameraUBO = nullptr; // render data( vertices )
	Mat44 m_projection;
	Mat44 m_view;
};
