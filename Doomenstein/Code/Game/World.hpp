#pragma once
class Map;
class World
{
public:
	World();
	~World(){}
	void Update(float deltaSeconds);	
	void Render()const ;
	//void DestroyEntities();

public:
	Map* m_currentMap = nullptr;
};

