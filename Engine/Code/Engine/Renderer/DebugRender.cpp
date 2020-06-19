#include "DebugRender.hpp"
#include <vector>
#include <stdarg.h>
#include <string>
#include "Engine/Core/Transform.hpp"
#include "Engine/Core/Time/Timer.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRenderObject.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

extern DevConsole* g_theConsole;

// static variable
static bool g_ableDebug = false;
static float g_sizeScale = 0.1f;
static float g_uiSizeCoe = 5.f;
static float g_2dZ = -10.f;
static float g_aspectRatio = 1.f;
static float g_basisCylinderLength = 0.7f;
static float g_basisConeLength = 0.3f;
static float g_basisCylinderRadius = 0.01f;
static float g_basisConeRadius = 0.05f;
static float g_defaultTextSize = 2.f;

static Vec2 g_screenMax = Vec2( 1.f, 1.f );
static Vec2 g_screenMin = Vec2::ZERO;

static Rgba8 g_defaultColor;
static Clock* g_clock;
static BitmapFont* g_defaultDebugFont = nullptr;

static GPUMesh* g_sphereMesh;
static GPUMesh* g_lineMesh;
static GPUMesh* g_cubeMesh;
static GPUMesh* g_arrowMesh;
static GPUMesh* g_basisMesh;
static GPUMesh* g_basisMesh2D;

static Camera* g_camera = nullptr;
static RenderContext* g_ctx = nullptr;

static std::vector<Vertex_PCU> g_vertices;
static std::vector<DebugRenderObject*> g_debugAlwaysObject;
static std::vector<DebugRenderObject*> g_debugDepthObject;
static std::vector<DebugRenderObject*> g_debugXrayObject;
static std::vector<DebugRenderObject*> g_debugScreenObject;

static Convention g_worldConvention = X_FORWARD_Y_LEFT_Z_UP;

// Decleration
void CreateGPUMeshes();
void CreateBitmapFont();
void DebugRenderAlwaysObjects();
void DebugRenderDepthObjects();
void DebugRenderXraysObjects();
void DebugRenderScreenObjects();
void DebugRenderOneObject( DebugRenderObject* object, float xrayTint = 1 );
void AddObjectToVectors( DebugRenderObject* object, DebugRenderMode mode ); 
void CleanOldObjects();
void CleanOldObjectsInVector( std::vector<DebugRenderObject*>& vec );

// private
void CreateGPUMeshes()
{
	g_sphereMesh	= new GPUMesh();
	g_lineMesh		= new GPUMesh();
	g_cubeMesh		= new GPUMesh();
	g_arrowMesh		= new GPUMesh();
	g_basisMesh		= new GPUMesh();
	g_basisMesh2D	= new GPUMesh();

	g_sphereMesh->m_debugMsg = "g_sphereMesh";
	g_lineMesh->m_debugMsg = "g_lineMesh";
	g_cubeMesh->m_debugMsg = "g_cubeMesh";
	g_arrowMesh->m_debugMsg = "g_arrowMesh";
	g_basisMesh->m_debugMsg = "g_basisMesh";
	g_basisMesh2D->m_debugMsg = "g_basisMesh2D";

	// sphere
	std::vector<Vertex_PCU> sphereVertices;
	std::vector<uint> sphereIndices;
	AppendIndexedVertsForSphere3D( sphereVertices, sphereIndices, Vec3( 0.f ), 1.f, 32, 16, Rgba8::WHITE );
	g_sphereMesh->UpdateVerticesInCPU( sphereVertices );
	g_sphereMesh->UpdateIndicesInCPU( sphereIndices );

	// cube
	AABB3 cube = AABB3( Vec3( -0.5f, -0.5f, -0.5f ), Vec3( 0.5f, 0.5f, 0.5f ) );
	std::vector<Vertex_PCU> cubeVertices;
	std::vector<uint> cubeIndices;
	AppendIndexedVertsForAABB3D( cubeVertices, cubeIndices, cube, Rgba8::WHITE );
	g_cubeMesh->UpdateVerticesInCPU( cubeVertices );
	g_cubeMesh->UpdateIndicesInCPU( cubeIndices );

	// line
	std::vector<Vertex_PCU> lineVertices;
	std::vector<uint> lineIndices;
	Cylinder3 cylinder = Cylinder3( Vec3::ZERO, Vec3( 0.f, 0.f, -1.f ), 1.f );
	AppendIndexedVertsForCylinder3D( lineVertices, lineIndices, cylinder, 12, Rgba8::WHITE, Rgba8::WHITE );

	g_lineMesh->UpdateVerticesInCPU( lineVertices );
	g_lineMesh->UpdateIndicesInCPU( lineIndices );

	// arrow
	std::vector<Vertex_PCU> arrowVertices;
	std::vector<uint> arrowIndices;
	Cone3 cone = Cone3( Vec3( 0.f, 0.f, -1.f ), Vec3( 0.f, 0.f, -1.5f ), 2.f );
	AppendIndexedVertsForCylinder3D( arrowVertices, arrowIndices, cylinder, 12, Rgba8::WHITE, Rgba8::WHITE );
	AppendIndexedVertsForCone3D( arrowVertices, arrowIndices, cone, 12, Rgba8::WHITE );

	g_arrowMesh->UpdateVerticesInCPU( arrowVertices );
	g_arrowMesh->UpdateIndicesInCPU( arrowIndices );

	// basis
	std::vector<Vertex_PCU> basisVertices;
	std::vector<uint> basisIndices;
	Cone3 coneX = Cone3( Vec3( g_basisCylinderLength, 0.f, 0.f ), Vec3( ( g_basisCylinderLength + g_basisConeLength ), 0.f, 0.f ), g_basisConeRadius );
	Cone3 coneY = Cone3( Vec3( 0.f, g_basisCylinderLength, 0.f ), Vec3( 0.f, ( g_basisCylinderLength + g_basisConeLength ), 0.f ), g_basisConeRadius );
	Cone3 coneZ = Cone3( Vec3( 0.f, 0.f, g_basisCylinderLength ), Vec3( 0.f, 0.f, ( g_basisCylinderLength + g_basisConeLength ) ), g_basisConeRadius );
	Cylinder3 cylinderX = Cylinder3( Vec3::ZERO, Vec3( g_basisCylinderLength, 0.f, 0.f ), g_basisCylinderRadius );
	Cylinder3 cylinderY = Cylinder3( Vec3::ZERO, Vec3( 0.f, g_basisCylinderLength, 0.f ), g_basisCylinderRadius );
	Cylinder3 cylinderZ = Cylinder3( Vec3::ZERO, Vec3( 0.f, 0.f, g_basisCylinderLength ), g_basisCylinderRadius );

	AppendIndexedVertsForCylinder3D( basisVertices, basisIndices, cylinderX, 12, Rgba8::RED, Rgba8::RED );
	AppendIndexedVertsForCone3D( basisVertices, basisIndices, coneX, 12, Rgba8::HALFRED, Rgba8( 255, 0, 0, 0 ) );
	AppendIndexedVertsForCylinder3D( basisVertices, basisIndices, cylinderY, 12, Rgba8::GREEN, Rgba8::GREEN );
	AppendIndexedVertsForCone3D( basisVertices, basisIndices, coneY, 12, Rgba8::HALFGREEN, Rgba8( 0, 255, 0, 0 ) );
	AppendIndexedVertsForCylinder3D( basisVertices, basisIndices, cylinderZ, 12, Rgba8::BLUE, Rgba8::BLUE );
	AppendIndexedVertsForCone3D( basisVertices, basisIndices, coneZ, 12, Rgba8::HALFBLUE, Rgba8( 0, 0, 255, 0 ) );

	g_basisMesh->UpdateVerticesInCPU( basisVertices );
	g_basisMesh->UpdateIndicesInCPU( basisIndices );

	// basis 2D
	std::vector<Vertex_PCU> basis2DVertices;
	std::vector<uint> basis2DIndices;

	AppendIndexedVertsForCylinder3D( basis2DVertices, basis2DIndices, cylinderX, 12, Rgba8::RED, Rgba8::RED );
	AppendIndexedVertsForCylinder3D( basis2DVertices, basis2DIndices, cylinderY, 12, Rgba8::GREEN, Rgba8::GREEN );
	AppendIndexedVertsForCone3D( basis2DVertices, basis2DIndices, coneX, 12, Rgba8::RED );
	AppendIndexedVertsForCone3D( basis2DVertices, basis2DIndices, coneY, 12, Rgba8::GREEN );

	g_basisMesh2D->UpdateVerticesInCPU( basis2DVertices );
	g_basisMesh2D->UpdateIndicesInCPU( basis2DIndices );
}

void CreateBitmapFont()
{
	g_defaultDebugFont = g_ctx->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
}

void DebugRenderAlwaysObjects()
{
	g_ctx->DisableDepth();
	for( int i = 0; i < g_debugAlwaysObject.size(); i++ ) {
		DebugRenderObject* object = g_debugAlwaysObject[i];
		if( object == nullptr ) { continue; }
		DebugRenderOneObject( object );
	}
}

void DebugRenderDepthObjects()
{
	g_ctx->EnableDepth( COMPARE_DEPTH_LESS, true );
	for( int i = 0; i < g_debugDepthObject.size(); i++ ) {
		DebugRenderObject* object = g_debugDepthObject[i];
		if( object == nullptr ) { continue; }
		DebugRenderOneObject( object );
	}
}

void DebugRenderXraysObjects()
{
	g_ctx->EnableDepth( COMPARE_DEPTH_GREATER_EQUAL, false );
	for( int i = 0; i < g_debugXrayObject.size(); i++ ) {
		DebugRenderObject* object = g_debugXrayObject[i];
		if( object == nullptr ) { continue; }
		DebugRenderOneObject( object, 0.5f );
	}
	g_ctx->EnableDepth( COMPARE_DEPTH_LESS, true );
	for( int i = 0; i < g_debugXrayObject.size(); i++ ) {
		DebugRenderObject* object = g_debugXrayObject[i];
		if( object == nullptr ) { continue; }
		DebugRenderOneObject( object );
	}
}

void DebugRenderScreenObjects()
{
	for( int i = 0; i < g_debugScreenObject.size(); i++ ) {
		DebugRenderObject* object = g_debugScreenObject[i];
		if( object == nullptr ) { continue; }
		DebugRenderOneObject( object );
	}
}

void DebugRenderOneObject( DebugRenderObject* object, float xrayTint )
{
	if( object->m_type == OBJECT_TEXT ) {
		Texture* textTex = g_defaultDebugFont->GetTexture();
		g_ctx->SetDiffuseTexture( textTex );
	}
	if( object->m_type == OBJECT_BILLBOARD_TEXT ) {
		Texture* textTex = g_defaultDebugFont->GetTexture();
		g_ctx->SetDiffuseTexture( textTex );
		object->m_transform.SetPitchDegrees( g_camera->m_transform.GetPitchDegrees() );
		object->m_transform.SetRollDegrees( g_camera->m_transform.GetRollDegrees() );
		object->m_transform.SetYawDegrees( g_camera->m_transform.GetYawDegrees() );
	}
	if( object->m_type == OBJECT_QUAD && object->m_texture != nullptr ) {
		g_ctx->SetDiffuseTexture( object->m_texture );
	}
	if( object->m_type == OBJECT_SCREEN_TEXT ) {
		Texture* textTex = g_defaultDebugFont->GetTexture();
		g_ctx->SetDiffuseTexture( textTex );
	}
	
	Rgba8 tintColor = object->GetTintColor( (float)g_clock->GetTotalSeconds() );
	tintColor.a = (unsigned char)(tintColor.a * xrayTint);
	g_ctx->SetTintColor( tintColor );
	
	object->RenderObject( g_ctx );
	g_ctx->SetDiffuseTexture( nullptr );
}

void AddObjectToVectors( DebugRenderObject* object, DebugRenderMode mode )
{
	switch( mode )
	{
	case DEBUG_RENDER_ALWAYS:
		g_debugAlwaysObject.push_back( object );
		break;
	case DEBUG_RENDER_USE_DEPTH:
		g_debugDepthObject.push_back( object );
		break;
	case DEBUG_RENDER_XRAY:
		g_debugXrayObject.push_back( object );
		break;
	case DEBUG_RENDER_SCREEN:
		g_debugScreenObject.push_back( object );
		break;
	}
}

void DebugRenderSystemStartup( RenderContext* ctx, Camera* camera )
{
	CreateGPUMeshes();
	g_ctx = ctx;
	g_clock = new Clock( g_ctx->m_clock );
	CreateBitmapFont();
	g_screenMax = Vec2( 160.f, 90.f );
	g_aspectRatio = 1.f;
	g_defaultColor = Rgba8::WHITE;
	g_camera = camera; // new camera

	// add command
	// hard to maintain
	std::string enableRenderComd			= std::string( "debug_render" );
	std::string AddWorldPointComd			= std::string( "debug_add_world_point" );
	std::string AddWorldWireSphereComd		= std::string( "debug_add_world_wire_sphere" );
	std::string AddWorldWireBoundComd		= std::string( "debug_add_world_wire_bound" );
	std::string AddWorldBillboardTextComd	= std::string( "debug_add_world_billboard_text" );
	std::string AddScreenPointComd			= std::string( "debug_add_screen_point" );
	std::string AddScreenQuadComd			= std::string( "debug_add_screen_quad" );
	std::string AddScreenTextComd			= std::string( "debug_add_screen_text" );

	std::string enableRenderComdDesc			= std::string( "enable debug render.(enabled=bool)" );
	std::string AddWorldPointComdDesc			= std::string( "point(position=vec3 duration=float)" );
	std::string AddWorldWireSphereComdDesc		= std::string( "wire sphere(position=vec3 radius=float duration=float)" );
	std::string AddWorldWireBoundComdDesc		= std::string( "wire bound(min=vec3 max=vec3 duration=float)" );
	std::string AddWorldBillboardTextComdDesc	= std::string( "billboard text(position=vec3 pivot=vec2 text=string)" );
	std::string AddScreenPointComdDesc			= std::string( "screen point(position=vec2 duration=float)" );
	std::string AddScreenQuadComdDesc			= std::string( "screen quad(min=vec2 max=vec2 duration=float)" );
	std::string AddScreenTextComdDesc			= std::string( "screen text(position=vec2 pivot=vec2 text=string)" );

 	EventCallbackFunctionPtr enableRenderFuncPtr			= DebugRenderCommandEnableDebugEvent;				
 	EventCallbackFunctionPtr AddWorldPointFuncPtr			= DebugRenderCommandAddWorldPointEvent;
 	EventCallbackFunctionPtr AddWorldWireSphereFuncPtr		= DebugRenderCommandAddWorldWireSphereEvent;
 	EventCallbackFunctionPtr AddWorldWireBoundFuncPtr		= DebugRenderCommandAddWorldWireBoundEvent;
 	EventCallbackFunctionPtr AddWorldBillboardTextFuncPtr	= DebugRenderCommandAddWorldBillboardTextEvent;
 	EventCallbackFunctionPtr AddScreenPointFuncPtr			= DebugRenderCommandAddScreenPointEvent;
 	EventCallbackFunctionPtr AddScreenQuadFuncPtr			= DebugRenderCommandAddScreenQuadEvent;	
 	EventCallbackFunctionPtr AddScreenTextFuncPtr			= DebugRenderCommandAddScreenTextEvent;	

	g_theConsole->AddCommandToCommandList( enableRenderComd, enableRenderComdDesc, enableRenderFuncPtr );
	g_theConsole->AddCommandToCommandList( AddWorldPointComd, AddWorldPointComdDesc, AddWorldPointFuncPtr );
	g_theConsole->AddCommandToCommandList( AddWorldWireSphereComd, AddWorldWireSphereComdDesc, AddWorldWireSphereFuncPtr );
	g_theConsole->AddCommandToCommandList( AddWorldWireBoundComd, AddWorldWireBoundComdDesc, AddWorldWireBoundFuncPtr );
	g_theConsole->AddCommandToCommandList( AddWorldBillboardTextComd, AddWorldBillboardTextComdDesc, AddWorldBillboardTextFuncPtr );
	g_theConsole->AddCommandToCommandList( AddScreenPointComd, AddScreenPointComdDesc, AddScreenPointFuncPtr );
	g_theConsole->AddCommandToCommandList( AddScreenQuadComd, AddScreenQuadComdDesc, AddScreenQuadFuncPtr );
	g_theConsole->AddCommandToCommandList( AddScreenTextComd, AddScreenTextComdDesc, AddScreenTextFuncPtr );
}

void DebugRenderSystemShutdown()
{
	SELF_SAFE_RELEASE(g_sphereMesh);
	SELF_SAFE_RELEASE(g_lineMesh);
	SELF_SAFE_RELEASE(g_cubeMesh);
	SELF_SAFE_RELEASE(g_arrowMesh);
	SELF_SAFE_RELEASE(g_basisMesh);
	SELF_SAFE_RELEASE(g_basisMesh2D);

	g_camera = nullptr;
	g_ctx	 = nullptr;
	g_vertices.clear();
}



void EnableDebugRendering()
{
	g_ableDebug = true;
}

void DisableDebugRendering()
{
	g_ableDebug = false;
}

void ClearDebugRendering()
{
	// what to do in clear
	//g_ctx->ClearState();
}

void DebugRenderBeginFrame()
{
	// what to do
	//g_clock->SelfBeginFrame();

}

void DebugRenderWorldToCamera( Camera* camera, Convention convention )
{
	if( !g_ableDebug ) { return; }

	g_camera = camera;
	uint clearState = g_camera->m_clearState;
	g_camera->SetClearMode( CLEAR_NONE );
	g_ctx->BeginCamera( g_camera, convention );

	DebugRenderDepthObjects();
	DebugRenderXraysObjects();
	DebugRenderAlwaysObjects(); 	

	g_ctx->EndCamera();
	camera->SetClearMode( clearState );
}

void DebugRenderScreenTo( Texture* output )
{
	if( !g_ableDebug ){ return; }

	// !!! not create every frame. create a new at start up.
	Camera* screenCamera = new Camera( g_ctx, 0.f, -100.f, g_screenMin, g_screenMax, g_aspectRatio );
	screenCamera->SetColorTarget( output );
	screenCamera->SetDepthStencilTarget( nullptr ); // don't need depth stencil target set target to nullptr
	screenCamera->SetClearMode( CLEAR_NONE );
	screenCamera->m_transform = Transform();
	screenCamera->SetUseDepth( false );

	g_ctx->BeginCamera( screenCamera, g_convention );
	DebugRenderScreenObjects();
	g_ctx->EndCamera();
	delete screenCamera;
}

void DebugRenderEndFrame()
{
	CleanOldObjects();
}

void CleanOldObjects()
{
	CleanOldObjectsInVector( g_debugAlwaysObject );	
	CleanOldObjectsInVector( g_debugDepthObject );	
	CleanOldObjectsInVector( g_debugXrayObject );	
	CleanOldObjectsInVector( g_debugScreenObject );	
}

void CleanOldObjectsInVector( std::vector<DebugRenderObject*>& vec )
{
	if( vec.size() == 1 && vec[0]->CheckIfOld() ){ 
		vec.clear();
		return;
	}

	for( int i = 0; i < vec.size(); i++ ) {
		DebugRenderObject* obj = vec[i];
		if( obj->CheckIfOld() ) {
			delete obj;
			vec[i] = vec.back();
			vec.pop_back();
			i--;
		}
	}
}

void DebugAddWorldPoint( Vec3 pos, float size, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugRenderObject* pointObject = DebugRenderObject::CreateObjectWithPosAndSize( pos, Vec3( size * g_sizeScale	 ), startColor, endColor, g_sphereMesh, g_clock, duration );
	pointObject->m_type = OBJECT_SPHERE;
	AddObjectToVectors( pointObject, mode );
}

void DebugAddWorldPoint( Vec3 pos, float size, Rgba8 color, float duration /*= 0.0f*/, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldPoint( pos, size, color, color, duration, mode );
}

void DebugAddWorldPoint( Vec3 pos, Rgba8 color, float duration /*= 0.0f*/, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldPoint( pos, 1, color, color, duration, mode );
}

void DebugAddWorldLine( Vec3 p0, Rgba8 p0_startColor, Rgba8 p0_endColor, Vec3 p1, Rgba8 p1_startColor, Rgba8 p1_endColor, float duration, DebugRenderMode mode )
{
	Cylinder3 cylinder = Cylinder3( p0, p1, 1.f );
	Vec3 size = Vec3( 1.f ) * g_sizeScale;
	size.z = cylinder.GetLength();
	//Mat44 lookAtMat = Mat44::GetLookAtMatrix( p0, p1 );

	DebugRenderObject* lineObject = DebugRenderObject::CreateObjectWithPosAndSize( p0, size, p0_startColor, p1_startColor, p0_endColor, p1_endColor, g_lineMesh, g_clock, duration );
	lineObject->m_transform.LookAt( p0, p1 );
	lineObject->m_type = OBJECT_CYLINDER;

	AddObjectToVectors( lineObject, mode );
}

void DebugAddWorldLine( Vec3 start, Vec3 end, Rgba8 color, float duration /*= 0.0f*/, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldLine( start, color, color, end, color, color, duration, mode );
}

void DebugAddWorldArrow( Vec3 p0, Rgba8 p0_startColor, Rgba8 p0_endColor, Vec3 p1, Rgba8 p1_startColor, Rgba8 p1_endColor, float duration, DebugRenderMode mode )
{
	Vec3 disp = p1 - p0;
	float dist = disp.GetLength();
	Vec3 size = Vec3( 1.f ) * g_sizeScale;
	size.z = dist;
	DebugRenderObject* arrowObject = DebugRenderObject::CreateObjectWithPosAndSize( p0, size, p0_startColor, p1_startColor, p0_endColor, p1_endColor, g_arrowMesh, g_clock, duration );
	arrowObject->m_transform.LookAt( p0, p1 );
	arrowObject->m_type = OBJECT_ARROW;

	AddObjectToVectors( arrowObject, mode );
}

void DebugAddWorldArrow( Vec3 start, Vec3 end, Rgba8 color, float duration, DebugRenderMode mode )
{
	DebugAddWorldArrow( start, color, color, end, color, color, duration, mode );
}

void DebugAddWorldQuad( Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode )
{
	std::vector<Vertex_PCU> vertices;
	Vertex_PCU v_p0 = Vertex_PCU( p0, startColor, Vec2::ZERO );
	Vertex_PCU v_p1 = Vertex_PCU( p1, startColor, Vec2::ZERO );
	Vertex_PCU v_p2 = Vertex_PCU( p2, startColor, Vec2::ZERO );
	Vertex_PCU v_p3 = Vertex_PCU( p3, startColor, Vec2::ZERO );

	vertices.push_back( v_p0 );
	vertices.push_back( v_p1 );
	vertices.push_back( v_p2 );

	vertices.push_back( v_p0 );
	vertices.push_back( v_p2 );
	vertices.push_back( v_p3 );

	DebugRenderObject* quadObject = DebugRenderObject::CreateObjectWithVertices( vertices, startColor, startColor, endColor, endColor, g_clock, duration );
	quadObject->m_type = OBJECT_QUAD;
	AddObjectToVectors( quadObject, mode );
}

void DebugAddWorldWireBounds( AABB3 bounds, Vec3 direction, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	Vec3 scale = bounds.max - bounds.min;
	Vec3 pos = bounds.GetCenter();
	Transform trans;
	trans.SetScale( scale );
	trans.LookAt( pos, pos + direction );
	
	DebugAddWorldWireBounds( trans.GetMatrix(), startColor, endColor, duration, mode );
}

void DebugAddWorldWireBounds( Mat44 trans, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWireMeshToWorld( trans, g_cubeMesh, startColor, duration, mode );
}

void DebugAddWorldWireSphere( Vec3 pos, float radius, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	Transform trans;
	trans.SetPosition( pos );
	trans.SetPitchDegrees( 0 );
	trans.SetYawDegrees( 0 );
	trans.SetRollDegrees( 0 );
	trans.SetScale( Vec3( radius ) );
	DebugAddWireMeshToWorld( trans.ToMatrix(), g_sphereMesh, startColor, endColor, duration, mode ); 
}

void DebugAddWorldWireSphere( Vec3 pos, float radius, Rgba8 color, float duration /*= 0.0f*/, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWorldWireSphere( pos, radius, color,color,duration, mode );
}

void DebugAddWorldBasis( Mat44 basis, Rgba8 startTint, Rgba8 endTint, float duration, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugRenderObject* basisObject = DebugRenderObject::CreateObjectWithMatrix( basis, startTint, endTint, g_basisMesh, g_clock, duration );
	basisObject->m_type = OBJECT_BASIS;

	AddObjectToVectors( basisObject, mode );
}

void DebugAddWorldText( Mat44 basis, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode, char const* text )
{
	std::string str = std::string( text );
	DebugAddWorldText( basis, pivot, startColor, endColor, duration, mode, str );
}

void DebugAddWorldText( Mat44 basis, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode, std::string text )
{
	std::vector<Vertex_PCU> vertices;
	float textHeight = 0.5f;
	Vec2 textDimension = g_defaultDebugFont->GetDimensionsForText2D( textHeight, text );
	AABB2 textBox = AABB2( Vec2::ZERO, textDimension );
	Vec2 offset = textBox.GetPointAtUV( pivot ); 
	g_defaultDebugFont->AddVertsForText2D( vertices, -offset, textHeight, text, Rgba8::WHITE );
	

	DebugRenderObject* textObject = DebugRenderObject::CreateObjectWithVertices( vertices, startColor, startColor, endColor, endColor, g_clock, duration );
	textObject->m_type = OBJECT_TEXT;
	textObject->m_transform.SetMatrix( basis );
	AddObjectToVectors( textObject, mode );
}

void DebugAddWorldText( Transform trans, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode, std::string text )
{
	std::vector<Vertex_PCU> vertices;
	float textHeight = 0.5f;
	Vec2 textDimension = g_defaultDebugFont->GetDimensionsForText2D( textHeight, text );
	AABB2 textBox = AABB2( Vec2::ZERO, textDimension );
	Vec2 offset = textBox.GetPointAtUV( pivot );
	g_defaultDebugFont->AddVertsForText2D( vertices, -offset, textHeight, text, Rgba8::WHITE );


	DebugRenderObject* textObject = DebugRenderObject::CreateObjectWithVertices( vertices, startColor, startColor, endColor, endColor, g_clock, duration );
	textObject->m_type = OBJECT_TEXT;
	textObject->m_transform = trans;
	textObject->m_transform.UpdateMatrix();
	AddObjectToVectors( textObject, mode );
}

void DebugAddWorldTextf( Mat44 basis, Vec2 pivot, Rgba8 color, float duration, DebugRenderMode mode, char const* text, ... )
{
	va_list args;
	va_start( args, text );
	std::string result = Stringv( text, args );
	va_end(args);
	DebugAddWorldText( basis, pivot, color, color, duration, mode, result );
}

void DebugAddWorldTextf( Mat44 basis, Vec2 pivot, Rgba8 color, float duration, char const* text, ... )
{
	va_list args;
	va_start( args, text );
	std::string result = Stringv( text, args );
	va_end( args );
	DebugAddWorldText( basis, pivot, color, color, duration, DEBUG_RENDER_USE_DEPTH, result ); 
}

void DebugAddWorldBillboardText( Vec3 origin, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode, char const* text )
{
	std::string str = std::string( text );
	DebugAddWorldBillboardText( origin, pivot, startColor, endColor, duration, mode, str );
}

void DebugAddWorldBillboardText( Vec3 origin, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode, std::string text )
{
	std::vector<Vertex_PCU> vertices;
	float textHeight = 0.5f;
	Vec2 textDimension = g_defaultDebugFont->GetDimensionsForText2D( textHeight, text );
	AABB2 textBox = AABB2( Vec2::ZERO, textDimension );
	Vec2 offset = textBox.GetPointAtUV( pivot );
	g_defaultDebugFont->AddVertsForText2D( vertices, -offset, textHeight, text, Rgba8::WHITE );

	DebugRenderObject* textObject = DebugRenderObject::CreateObjectWithVertices( vertices, startColor, startColor, endColor, endColor, g_clock, duration );
	textObject->m_type = OBJECT_BILLBOARD_TEXT;
	textObject->m_transform.SetPosition( origin );
	AddObjectToVectors( textObject, mode );
}

void DebugAddWorldBillboardText( Vec3 origin, Vec2 pivot, Rgba8 color, float duration, DebugRenderMode mode, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string result = Stringv( format, args );
	va_end(args);

	DebugAddWorldBillboardText( origin, pivot, color, color, duration, mode, result );
}

void DebugAddWorldBillboardText( Vec3 origin, Vec2 pivot, Rgba8 color, DebugRenderMode mode, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string result = Stringv( format, args );
	va_end( args );

	DebugAddWorldBillboardText( origin, pivot, color, color, 0.f, mode, result );
}

void DebugAddWireMeshToWorld( Mat44 transform, GPUMesh* mesh, Rgba8 startTint, Rgba8 endTint, float duration, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugRenderObject* object = DebugRenderObject::CreateObjectWithMatrix( transform, startTint, endTint, mesh, g_clock, duration );
	object->m_type = OBJECT_DEFAULT;
	object->m_useWire = true;

	AddObjectToVectors( object, mode );
}

void DebugAddWireMeshToWorld( Mat44 transform, GPUMesh* mesh, Rgba8 color, float duration, DebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DebugAddWireMeshToWorld( transform, mesh, color, color, duration, mode );
}

// void DebugRenderSetScreenHeight( float height )
// {
// 	
// }

AABB2 DebugGetScreenBounds()
{
	return AABB2( g_screenMin, g_screenMax );
}

void DebugAddScreenPoint( Vec2 pos, float size, Rgba8 startColor, Rgba8 endColor, float duration )
{
	Vec3 pos3 = Vec3( pos, -10.f );
	DebugRenderObject* pointObject = DebugRenderObject::CreateObjectWithPosAndSize( pos3, Vec3( size * g_sizeScale ), startColor, endColor, g_sphereMesh, g_clock, duration );
	pointObject->m_type = OBJECT_SPHERE;
	AddObjectToVectors( pointObject, DEBUG_RENDER_SCREEN );
}

void DebugAddScreenPoint( Vec2 pos, float size, Rgba8 color, float duration /*= 0.0f */ )
{
	DebugAddScreenPoint( pos, size, color, color, duration );
}

void DebugAddScreenPoint( Vec2 pos, Rgba8 color )
{
	DebugAddScreenPoint( pos, 1.f, color );
}

void DebugAddScreenLine( Vec2 p0, Rgba8 p0_startColor, Rgba8 p0_endColor, Vec2 p1, Rgba8 p1_startColor, Rgba8 p1_endColor, float duration )
{
	Cylinder3 cylinder = Cylinder3( p0, p1, -10.f );
	Vec3 size = Vec3( 1.f ) * g_sizeScale * g_uiSizeCoe;
	size.z = cylinder.GetLength();

	DebugRenderObject* lineObject = DebugRenderObject::CreateObjectWithPosAndSize( p0, size, p0_startColor, p1_startColor, p0_endColor, p1_endColor, g_lineMesh, g_clock, duration );
	lineObject->m_transform.LookAt( p0, p1 );
	lineObject->m_type = OBJECT_CYLINDER;

	AddObjectToVectors( lineObject, DEBUG_RENDER_SCREEN );
}

void DebugAddScreenLine( Vec2 p0, Vec2 p1, Rgba8 color, float duration /*= 0.0f */ )
{
	DebugAddScreenLine( p0, color, color, p1, color, color, duration );
}

void DebugAddScreenArrow( Vec2 p0, Rgba8 p0_startColor, Rgba8 p0_endColor, Vec2 p1, Rgba8 p1_startColor, Rgba8 p1_endColor, float duration )
{
	Vec3 v_p0 = Vec3( p0, g_2dZ );
	Vec3 v_p1 = Vec3( p1, g_2dZ );
	Vec3 disp = p1 - p0;
	float dist = disp.GetLength();
	Vec3 size = Vec3( 1.f ) * g_sizeScale * g_uiSizeCoe;
	size.z = dist;
	DebugRenderObject* arrowObject = DebugRenderObject::CreateObjectWithPosAndSize( p0, size, p0_startColor, p1_startColor, p0_endColor, p1_endColor, g_arrowMesh, g_clock, duration );
	arrowObject->m_transform.LookAt( p0, p1 );
	arrowObject->m_type = OBJECT_ARROW;

	AddObjectToVectors( arrowObject, DEBUG_RENDER_SCREEN );
}

void DebugAddScreenArrow( Vec2 p0, Vec2 p1, Rgba8 color, float duration /*= 0.0f */ )
{
	DebugAddScreenArrow( p0, color, color, p1, color, color, duration );
}

void DebugAddScreenQuad( AABB2 bounds, Rgba8 startColor, Rgba8 endColor, float duration )
{
	std::vector<Vertex_PCU> vertices;
	Vertex_PCU v_p0 = Vertex_PCU( bounds.mins, startColor, Vec2::ZERO );
	Vertex_PCU v_p1 = Vertex_PCU( Vec2( bounds.maxs.x, bounds.mins.y ), Rgba8::WHITE, Vec2::ZERO );
	Vertex_PCU v_p2 = Vertex_PCU( bounds.maxs, startColor, Vec2::ZERO );
	Vertex_PCU v_p3 = Vertex_PCU( Vec2( bounds.mins.x, bounds.maxs.y ), Rgba8::WHITE, Vec2::ZERO );

	vertices.push_back( v_p0 );
	vertices.push_back( v_p1 );
	vertices.push_back( v_p2 );

	vertices.push_back( v_p0 );
	vertices.push_back( v_p2 );
	vertices.push_back( v_p3 );

	DebugRenderObject* quadObject = DebugRenderObject::CreateObjectWithVertices( vertices, startColor, startColor, endColor, endColor, g_clock, duration );
	quadObject->m_type = OBJECT_QUAD;

	AddObjectToVectors( quadObject, DEBUG_RENDER_SCREEN );
}

void DebugAddScreenQuad( AABB2 bounds, Rgba8 color, float duration /*= 0.0f */ )
{
	DebugAddScreenQuad( bounds, color, color, duration );
}

void DebugAddScreenTexturedQuad( AABB2 bounds, Texture* tex, AABB2 uvs, Rgba8 startTint, Rgba8 endTint, float duration )
{
	std::vector<Vertex_PCU> vertices;
	Vertex_PCU v_p0 = Vertex_PCU( bounds.mins, Rgba8::WHITE, uvs.mins );
	Vertex_PCU v_p1 = Vertex_PCU( Vec2( bounds.maxs.x, bounds.mins.y ), Rgba8::WHITE, Vec2( uvs.maxs.x, uvs.mins.y ) );
	Vertex_PCU v_p2 = Vertex_PCU( bounds.maxs, Rgba8::WHITE	, uvs.maxs );
	Vertex_PCU v_p3 = Vertex_PCU( Vec2( bounds.mins.x, bounds.maxs.y ), Rgba8::WHITE, Vec2( uvs.mins.x, uvs.maxs.y ) );

	vertices.push_back( v_p0 );
	vertices.push_back( v_p1 );
	vertices.push_back( v_p2 );

	vertices.push_back( v_p0 );
	vertices.push_back( v_p2 );
	vertices.push_back( v_p3 );

	DebugRenderObject* quadObject = DebugRenderObject::CreateObjectWithVertices( vertices, startTint, startTint, endTint, endTint, g_clock, duration );
	quadObject->m_type = OBJECT_QUAD;
	quadObject->m_texture = tex;

	AddObjectToVectors( quadObject, DEBUG_RENDER_SCREEN );
}

void DebugAddScreenTexturedQuad( AABB2 bounds, Texture* tex, AABB2 uvs, Rgba8 tint, float duration /*= 0.0f */ )
{
	DebugAddScreenTexturedQuad( bounds, tex, uvs, tint, tint, duration );
}

void DebugAddScreenTexturedQuad( AABB2 bounds, Texture* tex, Rgba8 tint /*= Rgba8::WHITE*/, float duration /*= 0.0f */ )
{
	DebugAddScreenTexturedQuad( bounds, tex, AABB2( Vec2::ZERO, Vec2::ONE ), tint, tint, duration );
}

void DebugAddScreenText( Vec4 pos, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor, float duration, const char* text )
{
	std::vector<Vertex_PCU> vertices;
	Vec2 textDimension = g_defaultDebugFont->GetDimensionsForText2D( size, text );
	AABB2 screenBox = DebugGetScreenBounds();
	Vec2 textBoxMin = screenBox.GetPointAtUV( Vec2( pos.x, pos.y ) ) + Vec2( pos.z, pos.w ); 
	AABB2 textBox = AABB2( textBoxMin, ( textBoxMin + textDimension ) );
	Vec2 offset = textBox.GetPointAtUV( pivot );
	g_defaultDebugFont->AddVertsForTextInBox2D( vertices, textBox, size, text, Rgba8::WHITE );


	DebugRenderObject* textObject = DebugRenderObject::CreateObjectWithVertices( vertices, startColor, startColor, endColor, endColor, g_clock, duration );
	textObject->m_type = OBJECT_SCREEN_TEXT;
	AddObjectToVectors( textObject, DEBUG_RENDER_SCREEN );
}

void DebugAddScreenText( Vec4 pos, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor, float duration, std::string text )
{
	DebugAddScreenText( pos, pivot, size, startColor, endColor, duration, text.c_str() );
}

void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor, float duration, const char* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string result = Stringv( format, args );
	va_end( args );

	DebugAddScreenText( pos, pivot, size, startColor, endColor, duration, result );
}

void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, float size, Rgba8 color, float duration, const char* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string result = Stringv( format, args );
	va_end( args );

	DebugAddScreenText( pos, pivot, size, color, color, duration, result );
}

void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, float size, Rgba8 color, const char* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string result = Stringv( format, args );
	va_end( args );

	DebugAddScreenText( pos, pivot, size, color, color, 0.00000001f, result );
}

void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, Rgba8 color, const char* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string result = Stringv( format, args );
	va_end( args );

	DebugAddScreenText( pos, pivot, g_defaultTextSize, color, color, 0.f, result );
}

void DebugAddScreenRightAlignTextf( float relativeY, float absoluteY, Vec2 pivot, Rgba8 color, const char* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string result = Stringv( format, args );
	va_end( args );
	float textWidth = g_defaultDebugFont->GetWidthForText2D( g_defaultTextSize, result );
	DebugAddScreenText( Vec4( 1, relativeY, -textWidth, absoluteY ), pivot, g_defaultTextSize, color, color, 0.f, result );
	
}

void DebugAddScreenLeftAlignTextf( float relativeY, float absoluteY, Vec2 pivot, Rgba8 color, const char* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string result = Stringv( format, args );
	va_end( args );
	//float textWidth = g_defaultDebugFont->GetWidthForText2D( g_defaultTextSize, result );
	DebugAddScreenText( Vec4( 0.f, relativeY, 0.f, absoluteY ), pivot, g_defaultTextSize, color, color, 0.f, result );
}

void DebugAddScreenBasis( Vec2 screenOriginLocation, Mat44 basisToRender, Rgba8 startTint, Rgba8 endTint, float duration )
{
	Mat44 basis = basisToRender;
	basis.SetTranslation3D( Vec3( screenOriginLocation, g_2dZ ) );
	DebugRenderObject* basisObject = DebugRenderObject::CreateObjectWithMatrix( basis, startTint, endTint, g_basisMesh2D, g_clock, duration );
	basisObject->m_type = OBJECT_BASIS;

	AddObjectToVectors( basisObject, DEBUG_RENDER_SCREEN );
}

void DebugAddScreenBasis( Vec2 screenOriginLocation, Mat44 basisToRender, Rgba8 tint /*= Rgba8::WHITE*/, float duration /*= 0.0f */ )
{
	DebugAddScreenBasis( screenOriginLocation, basisToRender, tint, tint, duration );
}


// command
bool DebugRenderCommandEnableDebugEvent( EventArgs& args )
{
	bool arg1 = args.GetValue( std::to_string( 0 ), true );
	if( arg1 ){
		EnableDebugRendering();
	}
	else{
		DisableDebugRendering();
	}
	return true;
}

bool DebugRenderCommandAddWorldPointEvent( EventArgs& args )
{
	Vec3 position	= args.GetValue( std::to_string( 0 ), Vec3::ZERO );
	float duration	= args.GetValue( std::to_string( 1 ), 0.f );
	DebugAddWorldPoint( position, Rgba8::RED, duration );
	return true;
}

bool DebugRenderCommandAddWorldWireSphereEvent( EventArgs& args )
{
	Vec3 position	= args.GetValue( std::to_string( 0 ), Vec3::ZERO );
	float radius	= args.GetValue( std::to_string( 1 ), 1.f );
	float duration	= args.GetValue( std::to_string( 2 ), 1.f );

	DebugAddWorldWireSphere( position, radius, g_defaultColor, duration );
	return true;
}

bool DebugRenderCommandAddWorldWireBoundEvent( EventArgs& args )
{
	Vec3 min	= args.GetValue( std::to_string( 0 ), Vec3::ZERO );
	Vec3 max	= args.GetValue( std::to_string( 1 ), Vec3( Vec2::ZERO, -1.f ) );
	float duration	= args.GetValue( std::to_string( 2 ), 1.f );
	AABB3 bound = AABB3( min, max );
	DebugAddWorldWireBounds( bound, Vec3( 0.f, 0.f, -1.f ), g_defaultColor, g_defaultColor, duration );;
	return true;
}

bool DebugRenderCommandAddWorldBillboardTextEvent( EventArgs& args )
{
	Vec3 position		= args.GetValue( std::to_string( 0 ), Vec3::ZERO );
	Vec2 pivot			= args.GetValue( std::to_string( 1 ), Vec2::ZERO );
	std::string text	= args.GetValue( std::to_string( 2 ), std::string( " " ) );

	DebugAddWorldBillboardText( position, pivot, g_defaultColor, g_defaultColor, 5.f, DEBUG_RENDER_USE_DEPTH, text );
	return true;
}

bool DebugRenderCommandAddScreenPointEvent( EventArgs& args )
{
	Vec2 pos		= args.GetValue( std::to_string( 0 ), Vec2( 10.f, 10.f ) );
	float duration	= args.GetValue( std::to_string( 1 ), 5.f );

	DebugAddScreenPoint( pos, 10.f, g_defaultColor, duration );
	return true;
}

bool DebugRenderCommandAddScreenQuadEvent( EventArgs& args )
{
	Vec2 min		= args.GetValue( std::to_string( 0 ), Vec2( 10.f, 10.f ) );
	Vec2 max		= args.GetValue( std::to_string( 1 ), Vec2( 20.f, 20.f ) );
	float duration	= args.GetValue( std::to_string( 2 ), 5.f );

	AABB2 box = AABB2( min, max );
	DebugAddScreenQuad( box, g_defaultColor, duration );
	return true;
}

bool DebugRenderCommandAddScreenTextEvent( EventArgs& args )
{
	Vec2 position		= args.GetValue( std::to_string( 0 ), Vec2::ZERO );
	Vec2 pivot			= args.GetValue( std::to_string( 1 ), Vec2::ZERO );
	std::string text	= args.GetValue( std::to_string( 2 ), "" );

	Vec4 pos = Vec4( position.x, position.y, 0.f, 0.f );
	DebugAddScreenText( pos, pivot, 5.f, g_defaultColor, g_defaultColor, 5.f, text );
	return true;

}


