#include "MaterialsSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_theRenderer;

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

	Texture* tempSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( m_sheetTextures["Diffuse"].c_str() );
	m_diffuseTexture = tempSheetTexture;
	m_sheet = new SpriteSheet( *tempSheetTexture, m_layout );
}
