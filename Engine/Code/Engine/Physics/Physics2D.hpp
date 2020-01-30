#pragma once
#include <vector>

class Rigidbody2D;
class Collider2D;

class Physics2D {
public:
	void BeginFrame();
	void Update();
	void EndFrame();

	// factory style create/destroy
	Rigidbody2D* CreateRigidbody();
	void DestroyCollider( Collider2D* collider );

private:
	std::vector<Rigidbody2D*> m_rigidbodies;
	std::vector<Collider2D*> m_colliders;
};
