#pragma once
#include <vector>
#include "Engine/Math/Cylinder3.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Convention.hpp"

class Camera;

class Entity {
public:	
	Entity();
	~Entity(){}

public:
	//void Update( float deltaSeconds );
	void UpdateVerts();
	void UpdateTexCoords( const Camera* camera, BillboardMode billboardMode, Convention convention );

	// Accessor
	bool GetCanBePushedByWalls() const { return m_canBePushedByWalls; } 
	bool GetCanBePushedByEntities() const { return m_canBePushedByEntities; }
	bool GetCanPushEntities() const { return m_canPushEntities; }
	float GetMass() const { return m_mass; }
	Vec2 Get2DPosition() const { return m_2Dpos; }
	Vec3 GetPosition() const { return Vec3( m_2Dpos, 0.5f ); }

	// Mutator
	void SetCanBePushedByWalls( bool isAble );
	void SetCanBePushedByEntities( bool isAble );
	void SetCanPushEntities( bool isAble );
	void SetMass( float mass );
	void Set2DPos( Vec2 pos );
	void SetOrientation( float orientation );

	void SetTexForward( Vec3 texForward );
	void SetTexLeft( Vec3 texLeft );
	void SetTexUp( Vec3 texUp );

public:
	Cylinder3 m_cylinder;
	std::vector<Vertex_PCU> m_verts;
	Vec3 m_2Dpos		= Vec2::ZERO;
	Vec3 m_texForward	= Vec3::ZERO;
	Vec3 m_texLeft		= Vec3::ZERO;
	Vec3 m_texUp		= Vec3::ZERO;
	float m_orientation = 0;

private:
	bool m_canBePushedByWalls		= false;
	bool m_canBePushedByEntities	= false;
	bool m_canPushEntities			= false;
	float m_mass = 1.f;
};