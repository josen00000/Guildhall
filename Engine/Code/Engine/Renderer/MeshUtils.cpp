#include "MeshUtils.hpp"
#include "Engine/core/Vertex_PCU.hpp"
#include "Engine/Core/EngineCommon.hpp"

void AppendVertsForAABB2D( std::vector<Vertex_PCU>& vertices, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
{
	Vertex_PCU leftdown		= Vertex_PCU( bound.mins, tintColor, uvAtMins );
	Vertex_PCU rightup		= Vertex_PCU( bound.maxs, tintColor, uvAtMaxs );
	Vertex_PCU leftup		= Vertex_PCU( Vec2( bound.mins.x, bound.maxs.y ), tintColor, Vec2( uvAtMins.x, uvAtMaxs.y ) );
	Vertex_PCU rightdown	= Vertex_PCU( Vec2( bound.maxs.x, bound.mins.y ), tintColor, Vec2( uvAtMaxs.x, uvAtMins.y ) );

	// left up triangle
	vertices.push_back( leftdown );
	vertices.push_back( rightup );
	vertices.push_back( leftup );

	// right down triangle
	vertices.push_back( leftdown );
	vertices.push_back( rightup );
	vertices.push_back( rightdown );
}

void AppendVertsForCapsule2D( std::vector<Vertex_PCU>& vertices, const Capsule2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
{
	UNUSED(uvAtMins);
	UNUSED(uvAtMaxs);
	OBB2 capsuleOBB = bound.GetCapsuleOBB2();
	AppendVertsForOBB2D( vertices, capsuleOBB, tintColor, Vec2::ZERO, Vec2::ZERO );
	Vec2 direction = bound.GetNormalizedDirection();
	float orientationDegrees = direction.GetAngleDegrees();
	orientationDegrees += 90;
	orientationDegrees = ClampDegressTo360( orientationDegrees );
	AppendVertsForHalfCircle2D( vertices, bound.m_start, orientationDegrees, bound.m_radius, tintColor );
	orientationDegrees += 180;
	orientationDegrees = ClampDegressTo360( orientationDegrees );
	AppendVertsForHalfCircle2D( vertices, bound.m_end, orientationDegrees, bound.m_radius, tintColor );

}

void AppendVertsForOBB2D( std::vector<Vertex_PCU>& vertices, const OBB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
{
	Vertex_PCU leftDown = Vertex_PCU( bound.GetLeftDownCornerForIBasis(), tintColor, uvAtMins );
	Vertex_PCU leftUp = Vertex_PCU( bound.GetLeftUpCornerForIBasis(), tintColor, Vec2( uvAtMins.x, uvAtMaxs.y ) );
	Vertex_PCU rightDown = Vertex_PCU( bound.GetRightDownCornerForIBasis(), tintColor, Vec2( uvAtMaxs.x, uvAtMins.y ) );
	Vertex_PCU rightUp = Vertex_PCU( bound.GetrightUpCornerForIBasis(), tintColor, uvAtMaxs );

	//triangle 1
	vertices.push_back( leftDown );
	vertices.push_back( leftUp );
	vertices.push_back( rightUp );
	//triangle 2
	vertices.push_back( leftDown );
	vertices.push_back( rightDown );
	vertices.push_back( rightUp );
}

void AppendVertsForCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float radius, const Rgba8& tintColor )
{
	int triangleNumber = 64;
	float deltaDegrees = ( (float)360 / triangleNumber );
	float tempDegrees = 0; // current degrees
	float nextDegrees;
	Vec2 tempDegreesDirection = Vec2();
	Vec2 nextDegreesDirection = Vec2();
	Vertex_PCU centerVertex = Vertex_PCU( Vec3( center ), tintColor, Vec2::ZERO );

	for(int triangleIndex = 0; triangleIndex < triangleNumber; ++triangleIndex ){
		nextDegrees = tempDegrees + deltaDegrees;
		tempDegreesDirection.SetPolarDegrees( tempDegrees, 1 );
		nextDegreesDirection.SetPolarDegrees( nextDegrees, 1 );
		Vec2 tempDegreesPoint = center + tempDegreesDirection * radius;
		Vec2 nextDegreesPoint = center + nextDegreesDirection * radius;
		Vertex_PCU tempDegreesVertex = Vertex_PCU( Vec3( tempDegreesPoint ), tintColor, Vec2::ZERO );
		Vertex_PCU nextDegreesVertex = Vertex_PCU( Vec3( nextDegreesPoint ), tintColor, Vec2::ZERO );
		vertices.push_back( tempDegreesVertex );
		vertices.push_back( nextDegreesVertex );
		vertices.push_back( centerVertex );
		tempDegrees = nextDegrees;
	}
}

void AppendVertsForHalfCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float orientationDegrees, float radius, const Rgba8& tintColor )
{
	int triangleNumber = 11;
	float deltaDegrees = ( (float)180 / triangleNumber );
	float tempDegrees = orientationDegrees; // current degrees
	float nextDegrees;
	Vec2 tempDegreesDirection = Vec2();
	Vec2 nextDegreesDirection = Vec2();
	Vertex_PCU centerVertex = Vertex_PCU( Vec3( center ), tintColor, Vec2::ZERO );
	//triangleNumber+1
	for( int triangleIndex = 0; triangleIndex < triangleNumber; ++triangleIndex ) {
		nextDegrees = tempDegrees + deltaDegrees;
		tempDegreesDirection.SetPolarDegrees( tempDegrees, 1 );
		nextDegreesDirection.SetPolarDegrees( nextDegrees, 1 );
		Vec2 tempDegreesPoint = center + tempDegreesDirection * radius;
		Vec2 nextDegreesPoint = center + nextDegreesDirection * radius;
		Vertex_PCU tempDegreesVertex = Vertex_PCU( Vec3( tempDegreesPoint ), tintColor, Vec2::ZERO );
		Vertex_PCU nextDegreesVertex = Vertex_PCU( Vec3( nextDegreesPoint ), tintColor, Vec2::ZERO );
		vertices.push_back( tempDegreesVertex );
		vertices.push_back( nextDegreesVertex );
		vertices.push_back( centerVertex );
		tempDegrees = nextDegrees;
	}
}
