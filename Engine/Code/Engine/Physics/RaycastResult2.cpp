#include "RaycastResult2.hpp"

RaycastResult2::RaycastResult2( bool DidImpact, float ImpactDist, int ImpactTileType, Vec2 ImpactPos, Vec2 start )
	:m_didImpact(DidImpact)
	,m_impactDist(ImpactDist)
	,m_impactTileType(ImpactTileType)
	,m_impactPos(ImpactPos)
	,m_start(start)
	//,m_impactNormal(ImpactNormal)
{

}
