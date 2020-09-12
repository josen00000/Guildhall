#pragma once
#include <string>
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Camera;
class Texture;
class GPUMesh;
class DebugRenderObject;
class Transform;
class RenderContext;

struct AABB3;
struct AABB2;
struct Mat44;
struct OBB3;
struct Vec2;
struct Vec3;
struct Vec4;

enum DebugRenderMode {
	DEBUG_RENDER_ALWAYS,
	DEBUG_RENDER_USE_DEPTH,
	DEBUG_RENDER_XRAY,
	DEBUG_RENDER_SCREEN
};

//----------------------------------
// System
//----------------------------------

// setup
void DebugRenderSystemStartup( RenderContext* ctx, Camera* camera );
void DebugRenderSystemShutdown();

// control
void EnableDebugRendering();
void DisableDebugRendering();
void ClearDebugRendering();

// output
void DebugRenderBeginFrame();
void DebugRenderWorldToCamera( Camera* camera, Convention convention=X_RIGHT_Y_UP_Z_BACKWARD );
void DebugRenderScreenTo( Texture* output );
void DebugRenderEndFrame();

//----------------------------------
// World Rendering
//----------------------------------
// points
void DebugAddWorldPoint( Vec3 pos, float size, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldPoint( Vec3 pos, float size, Rgba8 color, float duration = 0.0f, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldPoint( Vec3 pos, Rgba8 color, float duration = 0.0f, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

// lines
void DebugAddWorldLine( Vec3 p0, Rgba8 p0_startColor, Rgba8 p0_endColor, Vec3 p1, Rgba8 p1_startColor, Rgba8 p1_endColor, float duration, DebugRenderMode mode );
void DebugAddWorldLine( Vec3 start, Vec3 end, Rgba8 color, float duration = 0.0f, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

// line strip
void DebugAddWorldLineStrip( int count, Vec3 const* positions, Rgba8 p0_startColor, Rgba8 p0_endColor, Rgba8 p1_startColor, Rgba8 p1_endColor, float duration, DebugRenderMode = DEBUG_RENDER_USE_DEPTH );

// arrows
void DebugAddWorldArrow( Vec3 p0, Rgba8 p0_startColor, Rgba8 p0_endColor, Vec3 p1, Rgba8 p1_startColor, Rgba8 p1_endColor, float duration, DebugRenderMode mode );
void DebugAddWorldArrow( Vec3 start, Vec3 end, Rgba8 color, float duration, DebugRenderMode mode );

// Quads
void DebugAddWorldQuad( Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, Rgba8 startColor, Rgba8 endColor, float duration ,DebugRenderMode mode );

// Bounds
void DebugAddWorldWireBounds( AABB3 bounds, Vec3 direction, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldWireBounds( Mat44 trans, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
//void DebugAddWorldWireBounds( OBB3 bounds, Rgba8 color, float duration = 0.0f, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
//void DebugAddWorldWireBounds( AABB3 bounds, Rgba8 color, float duration = 0.0f, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

void DebugAddWorldWireSphere( Vec3 pos, float radius, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldWireSphere( Vec3 pos, float radius, Rgba8 color, float duration = 0.0f, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

// basis
void DebugAddWorldBasis( Mat44 basis, Rgba8 startTint, Rgba8 endTint, float duration, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
//void DebugAddWorldBasis( Mat44 basis, Rgba8 startTint, Rgba8 endTint, float duration, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

// text
// non-billboarded will be oriented in the world based on the passed in basis matrix
void DebugAddWorldText( Mat44 basis, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode, char const* text );
void DebugAddWorldText( Mat44 basis, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode, std::string text );
void DebugAddWorldText( Transform trans, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode, std::string text );
void DebugAddWorldTextf( Mat44 basis, Vec2 pivot, Rgba8 color, float duration, DebugRenderMode mode, char const* text, ... );
void DebugAddWorldTextf( Mat44 basis, Vec2 pivot, Rgba8 color, float duration, char const* text, ... ); // assume DEBUG_RENDER_USE_DEPTH

// billboard orient to current camera
void DebugAddWorldBillboardText( Vec3 origin, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode, char const* text );
void DebugAddWorldBillboardText( Vec3 origin, Vec2 pivot, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode, std::string text );
void DebugAddWorldBillboardText( Vec3 origin, Vec2 pivot, Rgba8 color, float duration, DebugRenderMode mode, char const* format, ... );
void DebugAddWorldBillboardText( Vec3 origin, Vec2 pivot, Rgba8 color, DebugRenderMode mode, char const* format, ... );

// grid
void DebugAddWorldGrid( Vec3 origin,
						Vec3 i, float iMin, float iMax, float iMinorSegment, float iMajorSegment, Rgba8 iMinorColor, Rgba8 iMajorColor, Rgba8 iOriginColor,
						Vec3 j, float jMin, float jMax, float jMinorSegment, float jMajorSegment, Rgba8 jMinorColor, Rgba8 jMajorColor, Rgba8 jOriginColor,
						Rgba8 axisColor );
void DebugAddWorldXYGrid();
void DebugAddWorldXZGrid();
void DebugAddWorldYZGrid();

// mesh
void DebugAddWireMeshToWorld( Mat44 transform, GPUMesh* mesh, Rgba8 startTint, Rgba8 endTint, float duration, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWireMeshToWorld( Mat44 transform, GPUMesh* mesh, Rgba8 color, float duration, DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

//----------------------------------
// Screen Rendering
//----------------------------------
void DebugRenderSetScreenHeight( float height ); // default to 1080.0f when system starts up.  Meaning (0,0) should always be bottom left, (aspect * height, height) is top right
												 // useful if you want to align to top right for something
AABB2 DebugGetScreenBounds();

// points
void DebugAddScreenPoint( Vec2 pos, float size, Rgba8 startColor, Rgba8 endColor, float duration );
void DebugAddScreenPoint( Vec2 pos, float size, Rgba8 color, float duration = 0.0f );
void DebugAddScreenPoint( Vec2 pos, Rgba8 color ); // assume size;

// line
void DebugAddScreenLine( Vec2 p0, Rgba8 p0_startColor, Rgba8 p0_endColor,
						 Vec2 p1, Rgba8 p1_startColor, Rgba8 p1_endColor,
						 float duration );
void DebugAddScreenLine( Vec2 p0, Vec2 p1, Rgba8 color, float duration = 0.0f );

// arrows
void DebugAddScreenArrow( Vec2 p0, Rgba8 p0_startColor, Rgba8 p0_endColor,
						  Vec2 p1, Rgba8 p1_startColor, Rgba8 p1_endColor,
						  float duration );
void DebugAddScreenArrow( Vec2 p0, Vec2 p1, Rgba8 color, float duration = 0.0f );

// quad
void DebugAddScreenQuad( AABB2 bounds, Rgba8 startColor, Rgba8 endColor, float duration );
void DebugAddScreenQuad( AABB2 bounds, Rgba8 color, float duration = 0.0f );

// texture
void DebugAddScreenTexturedQuad( AABB2 bounds, Texture* tex, AABB2 uvs, Rgba8 startTint, Rgba8 endTint, float duration );
void DebugAddScreenTexturedQuad( AABB2 bounds, Texture* tex, AABB2 uvs, Rgba8 tint, float duration = 0.0f );
void DebugAddScreenTexturedQuad( AABB2 bounds, Texture* tex, Rgba8 tint = Rgba8::WHITE, float duration = 0.0f ); // assume uvs are full texture

// text
void DebugAddScreenText( Vec4 pos, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor, float duration, const char* text );
void DebugAddScreenText( Vec4 pos, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor, float duration, std::string text );
void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor, float duration, const char* format, ... );
void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, float size, Rgba8 color, float duration, const char* format, ... );
void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, float size, Rgba8 color, const char* format, ... );
void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, Rgba8 color, const char* format, ... );
void DebugAddScreenStrings( Vec4 pos, Vec2 povot, float size, Rgba8 color, Strings strings );

// align text
void DebugAddScreenLeftAlignText( float relativeY, float absoluteY, Vec2 pivot, Rgba8 color, float duration, std::string text );
void DebugAddScreenRightAlignTextf( float relativeY, float absoluteY, Vec2 pivot, Rgba8 color, const char* format, ... );
void DebugAddScreenLeftAlignTextf( float relativeY, float absoluteY, Vec2 pivot, Rgba8 color, const char* format, ... );
void DebugAddScreenLeftAlignStrings( float relativeY, float absoluteY, Rgba8 color, Strings strings );

// screen basis
void DebugAddScreenBasis( Vec2 screenOriginLocation, Mat44 basisToRender, Rgba8 startTint, Rgba8 endTint, float duration );
void DebugAddScreenBasis( Vec2 screenOriginLocation, Mat44 basisToRender, Rgba8 tint = Rgba8::WHITE, float duration = 0.0f );

// message log system
void DebugAddMessage( float duration, Rgba8 color, char const* format, ... );
void DebugAddMessage( Rgba8 color, char const* format, ... );


// Debug Command
bool DebugRenderCommandEnableDebugEvent( EventArgs& args );
bool DebugRenderCommandAddWorldPointEvent( EventArgs& args );
bool DebugRenderCommandAddWorldWireSphereEvent( EventArgs& args );
bool DebugRenderCommandAddWorldWireBoundEvent( EventArgs& args );
bool DebugRenderCommandAddWorldBillboardTextEvent( EventArgs& args );
bool DebugRenderCommandAddScreenPointEvent( EventArgs& args );
bool DebugRenderCommandAddScreenQuadEvent( EventArgs& args );
bool DebugRenderCommandAddScreenTextEvent( EventArgs& args );


