#include "GameObject.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

int GameObject::s_minPointNum	= 5;
int GameObject::s_maxPointNum	= 10;
int GameObject::s_maxXDist		= 5;
int GameObject::s_maxYDist		= 5;


RandomNumberGenerator GameObject::s_RNG = RandomNumberGenerator( 1 );

GameObject::GameObject( Vec2 pos )
{
	m_color = Rgba8::CYAN;
	int pointNum = s_RNG.RollRandomIntInRange( s_minPointNum, s_maxPointNum );
	std::vector<Vec2> polyPoints;
	for( int i = 0; i < pointNum; i++ ) {
		float xDist = s_RNG.RollRandomFloatInRange( -s_maxXDist, s_maxXDist );
		float yDist = s_RNG.RollRandomFloatInRange( -s_maxYDist, s_maxYDist );
		Vec2 point = pos + Vec2( xDist, yDist );
		polyPoints.push_back( point );
	}
	m_poly = ConvexPoly2::MakeConvexPolyFromPointCloud( polyPoints );
	m_hull = ConvexHull2::MakeConvexHullFromConvexPoly( m_poly );
}

void GameObject::UpdateObject( float deltaSeconds )
{
	Vec2 mousePos = g_theGame->GetMousePos();
	if( IsPointInside( mousePos ) ) {
		SetIsSelected( true );
		g_theGame->SetSelectedObject( this );
	}
	else {
		SetIsSelected( false );
		GameObject* selectedObj = g_theGame->GetSelectedObject();
		if( this == selectedObj ) {
			g_theGame->SetSelectedObject( nullptr );
		}
	}

	if( m_isSelected ) {
		m_color = Rgba8::RED;
	}
	else {
		 m_color = Rgba8::CYAN;
		 m_isRotating = false;
	}

	if( m_isRotating ) {
		std::vector<Vec2>& polyPoints = m_poly.GetPoints();
		float deltaDegree;
		switch ( m_rotateDirt )
		{
		case ROTATE_LEFT:
			deltaDegree = deltaSeconds * ROTATE_SPEED;
			break;
		case ROTATE_RIGHT:
			deltaDegree = deltaSeconds * ROTATE_SPEED * -1.f;
		}
		for( int i = 0; i < polyPoints.size(); i++ ) {
			Vec2 localDisp = polyPoints[i] - m_center;
			localDisp.RotateDegrees( deltaDegree );
			polyPoints[i] = m_center + localDisp;
		}
		m_hull = ConvexHull2::MakeConvexHullFromConvexPoly( m_poly );
	}
}

void GameObject::RenderObject()
{
	g_theRenderer->DrawConvexPoly2D( m_poly, m_color );
}

void GameObject::ScaleObjectWithPoint( float coe, Vec2 point )
{
	for( Vec2& tempPoint : m_poly.m_points ) {
		Vec2 disp = tempPoint - point;
		disp = disp * coe;
		tempPoint = point + disp;
	}
	m_hull = ConvexHull2::MakeConvexHullFromConvexPoly( m_poly );
}

bool GameObject::IsPointInside( Vec2 point )
{
	return m_hull.IsPointInside( point );
}

void GameObject::SetConvexPoly( ConvexPoly2 poly )
{
	m_poly = poly;
	m_hull = ConvexHull2::MakeConvexHullFromConvexPoly( m_poly );
}

void GameObject::SetIsSelected( bool isSelected )
{
	m_isSelected = isSelected;
}

void GameObject::SetIsRotating( bool isRotating )
{
	m_isRotating = isRotating;
}

void GameObject::SetRotateDirt( RotateDirection rotDirt )
{
	m_rotateDirt = rotDirt;
}

void GameObject::SetCenter( Vec2 point )
{
	m_center = point;
}
