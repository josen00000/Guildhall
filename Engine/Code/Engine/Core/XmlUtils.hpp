#pragma once
#include <string>
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include "Engine/Core/StringUtils.hpp"

struct Rgba8;
struct Vec2;
struct IntVec2;
struct FloatRange;
struct IntRange;

typedef	tinyxml2::XMLElement XmlElement;
typedef	tinyxml2::XMLDocument XmlDocument;
typedef tinyxml2::XMLAttribute XmlAttribute;

//std::vector<XmlElement> PopulateXmlDefinitionElementsFromXmlFiles( const char* xmlFilePath);
int			ParseXmlAttribute( const XmlElement& element, const char* attributeName, int defaultValue);
char		ParseXmlAttribute( const XmlElement& element, const char* attributeName, char defaultValue);
bool		ParseXmlAttribute( const XmlElement& element, const char* attributeName, bool defaultValue);
float		ParseXmlAttribute( const XmlElement& element, const char* attributeName, float defaultValue);
FloatRange	ParseXmlAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue);
IntRange	ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue);
Rgba8		ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Rgba8& defaultValue);
Vec2		ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue);
IntVec2		ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue);
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const std::string& defaultValue);
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const char* defaultValue);
Strings		ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Strings& defaultValue, const std::string delimiter);
const XmlElement* FindXmlChildElementWithName( const XmlElement& parentElement, const char* childElementName);


bool IfLoadXmlFileSucceed(const XmlDocument& xmlDocument);
bool IsElementNameValid( const XmlElement& element, std::string eleName );
bool IsElementNameValidWithStrings( const XmlElement& element, Strings eleNames );
