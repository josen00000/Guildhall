#pragma once
#include "Engine/Core/EngineCommon.hpp"

struct Vec2;

class Rigidbody2D;
class GameObject {
public:
	GameObject( Vec2 pos, float radius );
	~GameObject();

public:
	void Update( float deltaSeconds );
	void Render() const;
	void CheckIfMouseIn( Vec2 mousePos );

	// Accessor
	Vec2 GetPosition() const { return m_pos; }
	bool GetIfMouseIn() const { return m_isMouseIn; }
	bool GetIsSelected() const { return m_isSelected; }
	bool IsIntersectWith( GameObject* obj ) const;
	
	// Mutator
	void SetRigidbody( Rigidbody2D* rb );
	void SetPosition( Vec2 pos );
	void SetIsSelected( bool selected );
	void SetIsTouching( bool touching );

private:
	bool m_isDestroied	= false;
	bool m_isMouseIn	= false;
	bool m_isSelected	= false;
	bool m_isTouching	= false;
	Vec2 m_pos;
	float m_radius;
	Rigidbody2D* m_rb;
};
