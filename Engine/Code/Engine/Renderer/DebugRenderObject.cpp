#include "DebugRenderObject.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time/Timer.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"

DebugRenderObject::~DebugRenderObject()
{
	SELF_SAFE_RELEASE(m_timer);
}

DebugRenderObject* DebugRenderObject::CreateObject( Vec3 pos, Vec3 size, Rgba8 startColor, Rgba8 endColor, GPUMesh* mesh, Clock* clock, float duration )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = new Timer();
	object->m_timer->SetSeconds( clock, duration );
	object->m_mesh = mesh;
	object->m_transform.SetPosition( pos );
	object->m_transform.SetScale( size );
	object->m_startPosStartColor = startColor;
	object->m_endPosStartColor = startColor;
	object->m_endPosEndColor = endColor;
	object->m_startPosEndColor = endColor;
	return object;
}

DebugRenderObject* DebugRenderObject::CreateObject( Vec3 pos, Vec3 size, Rgba8 startPosStartColor, Rgba8 endPosStartColor, Rgba8 startPosEndColor, Rgba8 endPosEndColor, GPUMesh* mesh, Clock* clock, float duration )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = new Timer();
	object->m_timer->SetSeconds( clock, duration );
	object->m_mesh = mesh;
	object->m_transform.SetPosition( pos );
	object->m_transform.SetScale( size );
	object->m_startPosStartColor = startPosStartColor;
	object->m_endPosStartColor = endPosStartColor;
	object->m_endPosEndColor = endPosEndColor;
	object->m_startPosEndColor = startPosEndColor;
	return object;
}

DebugRenderObject* DebugRenderObject::CreateObject( Mat44 mat, Rgba8 startColor, Rgba8 endColor, GPUMesh* mesh, Clock* clock, float duration )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = new Timer();
	object->m_timer->SetSeconds( clock, duration );
	object->m_mesh = mesh;
	object->m_transform.mat = mat;
	object->m_startPosStartColor = startColor;
	object->m_endPosStartColor = startColor;
	object->m_endPosEndColor = endColor;
	object->m_startPosEndColor = endColor;
	return object;
}

DebugRenderObject* DebugRenderObject::CreateObject( std::vector<Vertex_PCU> vertices, Rgba8 startPosStartColor, Rgba8 endPosStartColor, Rgba8 startPosEndColor, Rgba8 endPosEndColor, Clock* clock, float duration )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_vertices = vertices;
	object->m_timer = new Timer();
	object->m_timer->SetSeconds( clock, duration );
	object->m_startPosStartColor = startPosStartColor;
	object->m_endPosStartColor = endPosStartColor;
	object->m_endPosEndColor = endPosEndColor;
	object->m_startPosEndColor = startPosEndColor;
	return object;
}

DebugRenderObject* DebugRenderObject::CreateObject( std::vector<Vertex_PCU> vertices, Transform trans, Rgba8 startPosStartColor, Rgba8 endPosStartColor, Rgba8 startPosEndColor, Rgba8 endPosEndColor, Clock* clock, float duration )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_vertices = vertices;
	object->m_transform = trans;
	object->m_timer = new Timer();
	object->m_timer->SetSeconds( clock, duration );
	object->m_startPosStartColor = startPosStartColor;
	object->m_endPosStartColor = endPosStartColor;
	object->m_endPosEndColor = endPosEndColor;
	object->m_startPosEndColor = startPosEndColor;
	return object;
}

bool DebugRenderObject::IsReadyToBeCulled() const
{
	return true;
}

bool DebugRenderObject::CheckIfOld()
{
	m_isOld = m_timer->HasElapsed();
	return m_isOld;
}

void DebugRenderObject::RenderObject( RenderContext* ctx )
{
	if( m_useWire ) {
		ctx->SetFillMode( RASTER_FILL_WIREFRAME );
	}
	ctx->SetModelMatrix( Mat44() );

	switch( m_type )
	{
	case OBJECT_DEFAULT:
		m_mesh->m_owner = ctx;
		ctx->SetModelMatrix( m_transform.GetMatrix() );
		ctx->DrawMesh( m_mesh );
		break;
	case OBJECT_SPHERE:
		m_mesh->m_owner = ctx;
		ctx->SetModelMatrix( m_transform.ToMatrix() );
		ctx->DrawMesh( m_mesh );
		break;
	case OBJECT_CYLINDER:
		m_mesh->m_owner = ctx;
		ctx->SetModelMatrix( m_transform.GetMatrix() );
		ctx->DrawMesh( m_mesh );
		break;
	case OBJECT_CUBE:
		m_mesh->m_owner = ctx;
		ctx->SetModelMatrix( m_transform.GetMatrix() );
		ctx->DrawMesh( m_mesh );
		break;
	case OBJECT_ARROW:
		m_mesh->m_owner = ctx;
		ctx->SetModelMatrix( m_transform.GetMatrix() );
		ctx->DrawMesh( m_mesh );
		break;
	case OBJECT_BASIS:
		m_mesh->m_owner = ctx;
		ctx->SetModelMatrix( m_transform.GetMatrix() );
		ctx->DrawMesh( m_mesh );
		break;
	case OBJECT_QUAD:
		ctx->SetModelMatrix( Mat44() );
		ctx->DrawVertexVector( m_vertices );
		break;
	case OBJECT_TEXT:
		ctx->SetModelMatrix( m_transform.ToMatrix() );
		ctx->DrawVertexVector( m_vertices );
		break;
	case OBJECT_BOARD_TEXT:
		ctx->SetModelMatrix( m_transform.ToMatrix() );
		ctx->DrawVertexVector( m_vertices );
		break;
	case OBJECT_SCREEN_TEXT:
		ctx->DrawVertexVector( m_vertices );
	default:
		break;
	}

// 	if( m_useMesh ){
// 		m_mesh->m_owner = ctx;
// 		if( m_type == OBJECT_CYLINDER || m_type == OBJECT_ARROW || m_type == OBJECT_BASIS || m_type == OBJECT_DEFAULT ) {
// 			//ctx->SetModelMatrix( Mat44() );
// 			ctx->SetModelMatrix( m_transform.GetMatrix() );
// 		}
// 		else {
// 			ctx->SetModelMatrix( m_transform.ToMatrix() );
// 		}
// 		ctx->DrawMesh( m_mesh );
// 	}
// 	else{
// 		if( m_type == OBJECT_TEXT ) {
// 
// 		}
// 		ctx->SetModelMatrix( Mat44() );
// 		ctx->DrawVertexVector( m_vertices );
// 	}

	ctx->SetFillMode( RASTER_FILL_SOLID );
}

Rgba8 DebugRenderObject::GetTintColor( float time )
{
	float deltaSeconds = time - (float)m_timer->m_startSeconds;
	float value = deltaSeconds / (float)m_timer->m_durationSeconds;
	Rgba8 tintColor;
	tintColor = Rgba8::GetLerpColor( m_startPosStartColor, m_startPosEndColor, value );
	return tintColor;
}
