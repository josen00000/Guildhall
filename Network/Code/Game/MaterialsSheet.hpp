#pragma once
#include <string>
#include <map>
#include "Engine/Renderer/SpriteSheet.hpp"
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
	Texture*	m_diffuseTexture = nullptr;
	std::map<std::string, std::string> m_sheetTextures;
	SpriteSheet*		m_sheet = nullptr; // temp use for stage 1
};