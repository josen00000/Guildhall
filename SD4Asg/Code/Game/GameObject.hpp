#pragma once
#include "Engine/Math/ConvexPoly2.hpp"
#include "Engine/Math/ConvexHull2.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/GameCommon.hpp"

class GameObject {
public:
	GameObject(){}
	~GameObject(){}
	GameObject( Vec2 pos );

	static RandomNumberGenerator s_RNG;

	void UpdateObject( float deltaSeconds );
	void RenderObject();
	void ScaleObjectWithPoint( float coe, Vec2 point );

	// Accessor
	bool IsPointInside( Vec2 point );
	ConvexPoly2 GetPoly() const{ return m_poly; }

	// Mutator
	void SetConvexPoly( ConvexPoly2 poly );
	void SetIsSelected( bool isSelected );
	void SetIsRotating( bool isRotating );
	void SetRotateDirt( RotateDirection rotDirt );
	void SetCenter( Vec2 point );

	static int s_minPointNum;
	static int s_maxPointNum;
	static int s_maxXDist;
	static int s_maxYDist;


private:
	bool			m_isSelected = false;
	bool			m_isRotating = false;
	RotateDirection	m_rotateDirt;
	Vec2			m_center;
	Rgba8			m_color;
	Transform		m_trans;
	ConvexPoly2		m_poly;
	ConvexHull2		m_hull;

	
};

