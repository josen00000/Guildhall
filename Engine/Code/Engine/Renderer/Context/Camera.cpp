#pragma once
#include <windows.h>
#include "Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Context/RenderContext.hpp"
#include "Engine/Renderer/GPU/RenderBuffer.hpp"
#include "Engine/Renderer/Context/RenderContext.hpp"

//extern HWND g_hWnd;
extern RenderContext* g_theRenderer;


Camera::Camera( const Vec2& bottomLeft, const Vec2& topRight, float aspectRatio )
{
	m_transform = Transform();
	SetOrthoView( bottomLeft, topRight, aspectRatio );
	m_projectionType = PROJECTION_ORTHOGRAPHIC;
}

Camera::Camera( float fov/*=60.f*/, float nearZ/*=-0.1*/, float farZ/*=-100*/, Vec3 pos, Vec3 rotPRY, Vec3 scale, const char* debugMsg )
{
	m_transform = Transform( pos, rotPRY, scale );
	SetProjectionPerspective( fov, nearZ, farZ );
	m_projectionType = PROJECTION_PERSPECTIVE;
	m_debugString = debugMsg;
}

Camera::Camera( float nZ, float fZ, const Vec2& bottomLeft/*=Vec2::ZERO*/, const Vec2& topRight/*=Vec2::ONE*/, float aspectRatio/*=1 */ )
{
	m_transform = Transform();
	SetOrthoView( bottomLeft, topRight, nZ, fZ, aspectRatio );
	m_projectionType = PROJECTION_ORTHOGRAPHIC;
}

Camera::Camera( float nZ, float fZ, const Vec2& bottomLeft/*=Vec2::ZERO*/, const Vec2& topRight/*=Vec2::ONE*/, float aspectRatio/*=1*/, const char* debugMsg /*= "" */ )
{
	m_transform = Transform();
	SetOrthoView( bottomLeft, topRight, nZ, fZ, aspectRatio );
	m_debugString = debugMsg;
	m_projectionType = PROJECTION_ORTHOGRAPHIC;
}

Camera::Camera( const char* debugMsg, const Vec2& bottomLeft/*=Vec2::ZERO*/, const Vec2& topRight/*=Vec2::ONE*/, float aspectRatio/*=1 */ )
{
	m_transform = Transform();
	SetOrthoView( bottomLeft, topRight, aspectRatio );
	m_debugString = debugMsg;
	m_projectionType = PROJECTION_ORTHOGRAPHIC;
}

Camera* Camera::CreateOrthographicCamera( const Vec2& bottomLeft, const Vec2& topRight )
{
	Camera* result = new Camera( bottomLeft, topRight );
	return result;
}

Camera* Camera::CreatePerspectiveCamera( float fov, float nearZ, float farZ, Vec3 pos, Vec3 rotPRY, Vec3 scale )
{
	Camera* result = new Camera( fov, nearZ, farZ, pos, rotPRY, scale );
	return result;
}

Camera* Camera::CreatePerspectiveCamera( const char* debugMsg, float fov, float nearZ, float farZ, Vec3 pos/*=Vec3::ZERO*/, Vec3 rotPRY/*=Vec3::ZERO*/, Vec3 scale/*=Vec3::ONE */ )
{
	UNUSED(debugMsg);
	Camera* result = new Camera( fov, nearZ, farZ, pos, rotPRY, scale );
	return result;
}

void Camera::Translate( const Vec3& translation )
{
	UNUSED( translation );
}

Camera::~Camera()
{
	if( m_cameraUBO != nullptr ) {
		delete m_cameraUBO;
		m_cameraUBO = nullptr;
	}
}


// Accessor
float Camera::GetCameraHeight() const
{
	return m_dimension.y;
}

float Camera::GetCameraWidth() const
{
	return m_dimension.x;
}

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight, float aspectRatio )
{
	m_dimension.x = topRight.x - bottomLeft.x;
	m_dimension.y = topRight.y - bottomLeft.y;
	m_projection = Mat44::CreateOrthographicProjectionMatrix( Vec3( bottomLeft, 0.f ), Vec3( topRight, -1.f ), aspectRatio );
}

void Camera::SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight, float nearZ, float farZ, float aspectRatio )
{
	m_dimension.x = topRight.x - bottomLeft.x;
	m_dimension.y = topRight.y - bottomLeft.y;
	m_projection = Mat44::CreateOrthographicProjectionMatrix( Vec3( bottomLeft, nearZ ), Vec3( topRight, farZ ), aspectRatio );
}

Vec3 Camera::GetPosition() const
{
	return m_transform.GetPosition();
}

RenderContext* Camera::GetRenderContext() const
{
	return m_rctx;
}

float Camera::GetOutputAspectRatio()
{
	IntVec2 outputSize = GetColorTarget()->GetTexelSize();
	float ratio = ( (float)outputSize.x / (float)outputSize.y );
	return ratio;
}

AABB2 Camera::GetCameraAsBox() const
{
	AABB2 result = AABB2( GetOrthoMin(), GetOrthoMax() );
	return result;
}

Mat44 Camera::GetUpdatedViewMatrix()
{
	UpdateViewMatrix();
	return m_view;
}

Mat44 Camera::GetModelMatrix() const
{
	return m_transform.ToMatrix();
}

bool Camera::IsClearColor() const
{
	return m_clearState & CLEAR_COLOR_BIT; 
}

bool Camera::IsClearDepth() const
{
	return m_clearState & CLEAR_DEPTH_BIT;
}

bool Camera::IsClearStencil() const
{
	return m_clearState & CLEAR_STENCIL_BIT;
}

void Camera::SetPosition( const Vec3& position )
{
	m_transform.SetPosition( position );
}

void Camera::SetRenderContext( RenderContext* ctx )
{
	m_rctx = ctx;
}

void Camera::SetProjectionOrthographic( float height, float nearZ /*= -1.0f*/, float farZ /*= 1.0f */ )
{
	float aspectRatio = GetOutputAspectRatio();
	Vec2 extents = Vec2( aspectRatio * height * 0.5f, height * 0.5f );
	Vec3 min = Vec3( -extents, nearZ );
	Vec3 max = Vec3( extents, farZ );
	//m_projection = Mat44::CreateOrthographicProjectionMatrix( min, max );
}

void Camera::SetUseDepth( bool useDepth )
{
	m_useDepth = useDepth;
}

void Camera::SetClearMode( uint clearFlags, Rgba8 color, float depth /*= 0.0f */, unsigned int stencil /*= 0 */ )
{
	m_clearState = clearFlags;
	m_clearColor = color;
	m_clearDepth = depth;
	m_clearStencil = stencil;
}

void Camera::EnableClearColor( Rgba8 color )
{
	m_clearColor = color;
	m_clearState = m_clearState | CLEAR_COLOR_BIT;
}

void Camera::EnableClearDepth( float depth )
{
	m_useDepth = true;
	m_clearDepth = depth;
	m_clearState = m_clearState | CLEAR_DEPTH_BIT;
}

void Camera::EnableClearStencil( uint stencil )
{
	m_clearStencil = stencil;
	m_clearState = m_clearState | CLEAR_STENCIL_BIT;
}

void Camera::DisableClearColor()
{
	m_clearState = m_clearState & ~CLEAR_COLOR_BIT;
}

void Camera::DisableClearDepth()
{
	m_clearState = m_clearState & ~CLEAR_DEPTH_BIT;
}

void Camera::DisableClearStencil()
{
	m_clearState = m_clearState & ~CLEAR_STENCIL_BIT;
}

void Camera::SetColorTarget( Texture* colorTarget, uint slot )
{
	if( slot >= m_colorTargets.size() ) {
		m_colorTargets.resize( slot +1 );
	}
	m_colorTargets[slot] = colorTarget;
}

void Camera::SetDepthStencilTarget( Texture* texture )
{
	m_depthStencilTarget = texture;
}

void Camera::SetPitchRollYawRotation( float pitch, float roll, float yaw )
{
	m_transform.SetRotationFromPitchRollYawDegrees( pitch, roll, yaw );
}

void Camera::SetProjectionPerspective( float fov/*=60*/, float nearZ/*=-0.1*/, float farZ/*=-100 */ )
{
	float aspect = GetOutputAspectRatio();
	m_projection = Mat44::CreatePerspectiveProjectionMatrix( fov, aspect, nearZ, farZ );
}

int Camera::GetRealColorTargetCount() const
{
	return (int)m_colorTargets.size();
}

int Camera::GetColorTargetCount() const
{
	if( m_colorTargets.size() == 0 ){ return 1; }

	return (int)m_colorTargets.size();
}

Texture* Camera::GetColorTarget( uint slot ) const
{
	//if( m_colorTargets != nullptr ){ return m_colorTargets; }
	if( slot < m_colorTargets.size() ){ return m_colorTargets[slot]; }

	return g_theRenderer->GetSwapChainBackBuffer();
}

Texture* Camera::GetOrCreateDepthStencilTarget( RenderContext* ctx )
{
	if( m_depthStencilTarget != nullptr ){
		return m_depthStencilTarget;
	}
	Texture* colorTarget = GetColorTarget();

	m_depthStencilTarget = Texture::CreateDepthStencilBuffer( ctx, colorTarget->GetTexelSize().x, colorTarget->GetTexelSize().y );
	return m_depthStencilTarget;
}

RenderBuffer* Camera::GetOrCreateCameraBuffer( RenderContext* ctx )
{
	static int i = 0;
	i++;
	m_debugString += std::to_string( i );
	if( m_cameraUBO == nullptr ) {
		m_cameraUBO = new RenderBuffer( ctx, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	}
	UpdateCameraUBO();
	return m_cameraUBO;
}

void Camera::UpdateCameraUBO()
{
	camera_ortho_t cameraData;
	cameraData.projection = m_projection; 
	UpdateViewMatrix();
	cameraData.view = m_view;
	cameraData.position = m_transform.GetPosition();
	m_cameraUBO->Update( &cameraData, sizeof( cameraData ), sizeof( cameraData ) );
}


void Camera::UpdateCameraRotation( Vec3 deltaRot )
{
	Vec3 cameraRot = m_transform.GetRotationPRYDegrees();
	cameraRot += deltaRot;
	cameraRot.x = ClampFloat( -85.f ,85.f, cameraRot.x );
	//cameraRot.x = ClampFloat();
	m_transform.SetRotationFromPitchRollYawDegrees( cameraRot );
}

Vec3 Camera::ClientToWorld( Vec2 client, float ndcZ )const
{
	Vec3 ndc = RangeMapVec3(  Vec3::ZERO, Vec3::ONE, Vec3( -1.f, -1.f, 0 ), Vec3( 1.f, 1.f, 1.f ),Vec3( client, ndcZ) );

	Mat44 proj = GetProjectionMatrix();
	Mat44 worldToClip = proj;
	worldToClip.Multiply( GetViewMatrix() );

	Mat44 clipToWorld = worldToClip.GetInvertMatrix(); // no bug
	Mat44 test = clipToWorld;
	test.Multiply( worldToClip );

	test = GetViewMatrix();



	Vec4 worldHomogenous = clipToWorld.TransformHomogeneousPoint3D( Vec4( ndc.x, ndc.y, ndc.z, 1.f) );
	Vec3 worldPos;
	worldPos.x = worldHomogenous.x / worldHomogenous.w;
	worldPos.y = worldHomogenous.y / worldHomogenous.w;
	worldPos.z = worldHomogenous.z / worldHomogenous.w;

	return worldPos;
}

Vec3 Camera::WorldToClient( Vec3 worldPos )
{
	return Vec3::ZERO;
}

void Camera::UpdateViewMatrix()
{
	// same as invertOrthonormal
	m_view = GetModelMatrix();
	m_view.SetTranslation3D( Vec3::ZERO );
	m_view.TransposeMatrix();

	Vec3 translation = m_transform.GetPosition();
	Vec3 inverse_translation = m_view.TransformPosition3D( -translation );
	m_view.SetTranslation3D( inverse_translation );
}

// Private
Vec2 Camera::GetOrthoMin() const
{
	Vec3 worldPos = ClientToWorld( Vec2( 0.f, 0.f ), 0.f );
	return Vec2( worldPos );
}

Vec2 Camera::GetOrthoMax() const
{
	Vec3 worldPos = ClientToWorld( Vec2( 1.f, 1.f ), 0.f );
	return Vec2( worldPos );

}