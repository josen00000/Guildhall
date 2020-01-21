#include "XmlUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"

/*
std::vector<XmlElement> PopulateXmlDefinitionElementsFromXmlFiles( const char* xmlFilePath )
{
	XmlDocument xmlDocument;
	std::vector<XmlElement> resultXmlElements;
	xmlDocument.LoadFile( xmlFilePath);
	if( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS ) {
		return resultXmlElements;
	}

	XmlElement* rootElement = xmlDocument.RootElement();
	if( !rootElement ) {
		return resultXmlElements;
	}

	XmlElement* childElement = rootElement->FirstChildElement();
	while( !childElement ){
		resultXmlElements.push_back(*childElement);


	}

}*/

std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const std::string& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	std::string value = defaultValue;
	if(attributeValueText){
		value = attributeValueText;
	}
	return value;
}

int ParseXmlAttribute( const XmlElement& element, const char* attributeName, int defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	int value = defaultValue;
	if( attributeValueText ) {
		value = GetIntFromText(attributeValueText);
	}
	return value;
}

char ParseXmlAttribute( const XmlElement& element, const char* attributeName, char defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	char value = defaultValue;
	if( attributeValueText ) {
		value = attributeValueText[0];
	}
	return value;
}

bool ParseXmlAttribute( const XmlElement& element, const char* attributeName, bool defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	bool value = defaultValue;
	if( attributeValueText ) {
		value = GetBoolFromText( attributeValueText );
	}
	return value;
}

float ParseXmlAttribute( const XmlElement& element, const char* attributeName, float defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	float value = defaultValue;
	if( attributeValueText ) {
		value = GetFloatFromText( attributeValueText ); 
	}
	return value;
}

Rgba8 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Rgba8& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	Rgba8 value = defaultValue;
	if( attributeValueText ) {
		value.SetFromText(attributeValueText);
	}
	return value;
}

Vec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	Vec2 value = defaultValue;
	if( attributeValueText ) {
		value.SetFromText( attributeValueText );
	}
	return value;
}

IntVec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	IntVec2 value = defaultValue;
	if( attributeValueText ) {
		value.SetFromText( attributeValueText );
	}
	return value;
}

Strings ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Strings& defaultValue, const std::string delimiter )
{
	const char* attributeValueText = element.Attribute( attributeName );
	Strings value = defaultValue;
	if( attributeValueText ) {
		value = SplitStringOnDelimiter( attributeValueText, delimiter );
	}
	return value;
}

std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const char* defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	std::string value = defaultValue;
	if( attributeValueText ) {
		value = attributeValueText;
	}
	return value;
}

FloatRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue )
{
	const char* attributeValueText =  element.Attribute( attributeName );
	FloatRange value = defaultValue;
	if( attributeValueText ){
		value.SetFromText( attributeValueText );
	}
	return value;
}

IntRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue )
{
	const char* attributeValueText =  element.Attribute( attributeName );
	IntRange value = defaultValue;
	if( attributeValueText ) {
		value.SetFromText( attributeValueText );
	}
	return value;
}

const XmlElement* FindXmlChildElementWithName( const XmlElement& parentElement, const char* childElementName )
{
	const XmlElement* childElement = parentElement.FirstChildElement();
	while( childElement ){
		std::string elementName = childElement->Name();
		if( elementName.compare( childElementName ) == 0 ){
			return childElement;
		}
		childElement = childElement->NextSiblingElement();
	}
	return nullptr;
}

bool IfLoadXmlFileSucceed( const XmlDocument& xmlDocument )
{
	if( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS ) {
		return false;
	}

	const XmlElement* rootElement = xmlDocument.RootElement();
	if( !rootElement ) {
		return false;
	}
	return true;
}
