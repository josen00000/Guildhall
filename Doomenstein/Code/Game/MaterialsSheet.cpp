#include "MaterialsSheet.hpp"

MaterialsSheet::MaterialsSheet( const XmlElement& materialSheetElement )
{
	m_name				= ParseXmlAttribute( materialSheetElement, "name", "" );
	m_layout			= ParseXmlAttribute( materialSheetElement, "layout", IntVec2( 0, 0 ) );

	const XmlElement* textureElement = materialSheetElement.FirstChildElement(); 
	while( textureElement ) {
		std::string textureName = textureElement->Name();
		std::string texturePath = ParseXmlAttribute( *textureElement, "image", "" );
		m_sheetTextures[textureName] = texturePath;
		textureElement = textureElement->NextSiblingElement();
	}
}
