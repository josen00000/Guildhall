#include<Game/Entity.hpp>
#include<Engine//Math/Vec3.hpp>
class Entity;
class PlayerShip;
class Bullet:public Entity {
public:
	//Constructor

	Bullet(){}
	~Bullet(){}
	Bullet(Game* iniGame, Vec2& iniPos,PlayerShip& iniPlayerShip,float forwardDegree);

public:
	void Startup();
	void Shutdown();
	virtual void Update(float deltaTime);
	const virtual void Render();
	virtual void Die();
	void CreateBullet();
	void GenerateDebris();
	Vec2 GetBulletVelocity();
public:
	Vertex_PCU m_shape[6];
	PlayerShip* m_PlayerShip=nullptr;
	float m_forwardDegree=0;
};