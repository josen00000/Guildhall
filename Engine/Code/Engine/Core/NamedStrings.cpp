#include "NamedStrings.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"

void NamedStrings::PopulateFromXmlElementAttributes( const XmlElement& element )
{
	const XmlAttribute* pAttr = element.FirstAttribute();
	while( pAttr){
		const char* keyName = pAttr->Name();
		const char* keyValue = pAttr->Value();
		m_keyValuePairs.insert( std::pair<std::string, std::string>(keyName, keyValue));
		pAttr = pAttr->Next();
	}
}

bool NamedStrings::PopulateFromXmlFile( const char* xmlFilePath )
{
	XmlDocument xmlDocument;
	xmlDocument.LoadFile( xmlFilePath );

	if(IfLoadXmlFileSucceed(xmlDocument)){
		XmlElement* rootElement = xmlDocument.RootElement();
		PopulateFromXmlElementAttributes( *rootElement );
		return true;
	}

	return false;
	
}

void NamedStrings::SetValue( const std::string& keyName, const std::string& newValue )
{
	m_keyValuePairs[keyName] = newValue;
}

bool NamedStrings::GetValue( const std::string& keyName, bool defaultValue ) const
{
	bool result = defaultValue;
	auto it = m_keyValuePairs.find(keyName);
	if(it != m_keyValuePairs.end()){
		std::string temResult = it->second;
		result = GetBoolFromText(temResult.c_str());
	}
	return result;
}

int NamedStrings::GetValue( const std::string& keyName, int defaultValue ) const
{
	int result = defaultValue;
	std::map< std::string, std::string>::const_iterator it = m_keyValuePairs.find( keyName );
	if( it != m_keyValuePairs.end() ) {
		std::string temResult = it->second;
		result = GetIntFromText(temResult.c_str());
	}
	return result;
}

float NamedStrings::GetValue( const std::string& keyName, float defaultValue ) const
{

	float result = defaultValue;
	auto it = m_keyValuePairs.find( keyName );
	if( it != m_keyValuePairs.end() ) {
		std::string temResult = it->second;
		result = GetFloatFromText( temResult.c_str() );
	}
	return result;

}

std::string NamedStrings::GetValue( const std::string& keyName, std::string defaultValue ) const
{

	std::string result = defaultValue;
	auto it = m_keyValuePairs.find( keyName );
	if( it != m_keyValuePairs.end() ) {
		result = it->second;
	}
	return result;

}

std::string NamedStrings::GetValue( const std::string& keyName, const char* defaultValue ) const
{
	std::string result = defaultValue;
	auto it = m_keyValuePairs.find( keyName );
	if( it != m_keyValuePairs.end() ) {
		result = it->second;
	}
	return result;
}

Rgba8 NamedStrings::GetValue( const std::string& keyName, const Rgba8& defaultValue ) const
{
	Rgba8 result = defaultValue;
	auto it = m_keyValuePairs.find( keyName );
	if( it != m_keyValuePairs.end() ) {
		std::string temResult = it->second;
		result.SetFromText( temResult.c_str() );
	}
	return result;
}

Vec2 NamedStrings::GetValue( const std::string& keyName, const Vec2& defaultValue ) const
{
	Vec2 result = defaultValue;
	auto it = m_keyValuePairs.find( keyName );
	if( it != m_keyValuePairs.end() ) {
		std::string temResult = it->second;
		result.SetFromText( temResult.c_str() );
	}
	return result;
}

IntVec2 NamedStrings::GetValue( const std::string& keyName, const IntVec2& defaultValue ) const
{
	IntVec2 result = defaultValue;
	auto it = m_keyValuePairs.find( keyName );
	if( it != m_keyValuePairs.end() ) {
		std::string temResult = it->second;
		result.SetFromText( temResult.c_str() );
	}
	return result;
}

