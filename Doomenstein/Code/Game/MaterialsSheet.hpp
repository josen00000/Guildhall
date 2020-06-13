#pragma once
#include <string>
#include <map>
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"

class MaterialsSheet {
public:
	MaterialsSheet(){}
	~MaterialsSheet(){}
	explicit MaterialsSheet( const XmlElement& MaterialElement );

public:
	std::string m_name = "";
	IntVec2		m_layout = IntVec2( 0, 0 );
	std::map<std::string, std::string> m_sheetTextures;
};