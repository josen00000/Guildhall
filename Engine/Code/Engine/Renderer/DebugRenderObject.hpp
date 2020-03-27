#pragma once
#include <vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Transform.hpp"

class Clock;
class GPUMesh;
class RenderContext;
class Texture;
class Timer;

struct Vertex_PCU;

enum RenderObjectType {
	OBJECT_DEFAULT,
	OBJECT_SPHERE,
	OBJECT_CYLINDER,
	OBJECT_CUBE,
	OBJECT_ARROW,
	OBJECT_BASIS,
	OBJECT_QUAD,
	OBJECT_TEXT,
	OBJECT_SCREEN_TEXT,
	OBJECT_BOARD_TEXT,
};


class DebugRenderObject {
public:
	DebugRenderObject(){}
	~DebugRenderObject();
	static DebugRenderObject* CreateObject( Vec3 pos, Vec3 size, Rgba8 startColor, Rgba8 endColor, GPUMesh* mesh, Clock* clock, float duration );
	static DebugRenderObject* CreateObject( Mat44 mat, Rgba8 startColor, Rgba8 endColor, GPUMesh* mesh, Clock* clock, float duration );
	static DebugRenderObject* CreateObject( Vec3 pos, Vec3 size, Rgba8 startPosStartColor, Rgba8 endPosStartColor,Rgba8 startPosEndColor, Rgba8 endPosEndColor, GPUMesh* mesh, Clock* clock, float duration );
	static DebugRenderObject* CreateObject( std::vector<Vertex_PCU> vertices, Rgba8 startPosStartColor, Rgba8 endPosStartColor,Rgba8 startPosEndColor, Rgba8 endPosEndColor, Clock* clock, float duration );
	static DebugRenderObject* CreateObject( std::vector<Vertex_PCU> vertices, Transform trans, Rgba8 startPosStartColor, Rgba8 endPosStartColor,Rgba8 startPosEndColor, Rgba8 endPosEndColor, Clock* clock, float duration );
	bool IsReadyToBeCulled() const;
	bool CheckIfOld();
	void RenderObject( RenderContext* ctx );
	Rgba8 GetTintColor( float time );

public:
	bool m_useMesh = true;
	bool m_isOld = false;
	bool m_useWire = false;
	
	Transform m_transform;
	GPUMesh* m_mesh = nullptr;
	Texture* m_texture = nullptr;
	
	Rgba8 m_startPosStartColor = Rgba8::WHITE;
	Rgba8 m_startPosEndColor = Rgba8::WHITE;
	Rgba8 m_endPosStartColor = Rgba8::WHITE;
	Rgba8 m_endPosEndColor = Rgba8::WHITE;
	
	Timer* m_timer;
	RenderObjectType m_type;
	std::vector<Vertex_PCU> m_vertices;
};