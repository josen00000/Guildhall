#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Game/EntityDefinition.hpp"
#include "Engine/Math/Cylinder3.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Convention.hpp"

class Camera;

class Entity {
public:
	Entity();
	~Entity(){}
public:	
	explicit Entity( const EntityDefinition& entityDef );

public:
	virtual void Update( float deltaSeconds );
	void UpdateVerts( Vec2 uvAtMin, Vec2 uvAtMax );
	void UpdateTexCoords( const Camera* camera, BillboardMode billboardMode, Convention convention );

	virtual void Render( ) const;

	// Accessor
	bool GetCanBePushedByWalls() const { return m_canBePushedByWalls; } 
	bool GetCanBePushedByEntities() const { return m_canBePushedByEntities; }
	bool GetCanPushEntities() const { return m_canPushEntities; }
	bool GetIsMoving() const { return m_isMoving; }
	bool GetIsPlayer() const { return m_isPlayer; }
	float GetMass() const { return m_mass; }
	float GetRadius() const { return m_definition->m_radius; }
	float GetHeight() const { return m_definition->m_height; }
	Vec2 Get2DPosition() const { return m_2Dpos; }
	Vec3 GetPosition() const { return Vec3( m_2Dpos, 0.5f ); }
	Vec2 GetforwardNormal2D() const;

	// Mutator
	void SetCanBePushedByWalls( bool isAble );
	void SetCanBePushedByEntities( bool isAble );
	void SetCanPushEntities( bool isAble );
	void SetIsMoving( bool isMoving ); 
	void SetIsPlayer( bool isPlayer );
	void SetMass( float mass );
	void Set2DPos( Vec2 pos );
	void SetOrientation( float orientation );
	void SetMoveDirt( Vec2 moveDirt );

	void SetTexForward( Vec3 texForward );
	void SetTexLeft( Vec3 texLeft );
	void SetTexUp( Vec3 texUp );

private:
	BillboardMode GetBillboardModeWithString( std::string billboardText );

public:
	Cylinder3 m_cylinder;
	std::vector<Vertex_PCU> m_verts;
	Vec3 m_2Dpos		= Vec2::ZERO;
	Vec3 m_texForward	= Vec3::ZERO;
	Vec3 m_texLeft		= Vec3::ZERO;
	Vec3 m_texUp		= Vec3::ZERO;
	float m_orientation = 0.f;
	Vec2 m_moveDirt		= Vec2::ZERO;
	
protected:
	bool m_canBePushedByWalls		= true;
	bool m_canBePushedByEntities	= true;
	bool m_canPushEntities			= true;

	bool m_isMoving = false;
	bool m_isPlayer = false;
	float m_mass = 1.f;

	const EntityDefinition* m_definition = nullptr;
};