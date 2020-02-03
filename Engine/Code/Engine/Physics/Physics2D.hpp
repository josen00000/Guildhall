#pragma once
#include <vector>

class Collider2D;
class DiscCollider2D;
class Rigidbody2D;
struct Vec2;

class Physics2D {
public:
	Physics2D(){}
	~Physics2D();

public:
	void BeginFrame();
	void Update();
	void EndFrame(); // Cleanup destroyed objects

	// factory style create/destroy
	Rigidbody2D* CreateRigidbody();
	void AddRigidbodyToList( Rigidbody2D* rb );
	void DestroyRigidbody( Rigidbody2D* rb );

	DiscCollider2D* CreateDiscCollider( Vec2 localPosition, float radius );
	void DestroyCollider( Collider2D* collider );
	void AddColliderToList( Collider2D* collider );

private:
	std::vector<Collider2D*> m_colliders;
	std::vector<Rigidbody2D*> m_rigidbodies;
};
