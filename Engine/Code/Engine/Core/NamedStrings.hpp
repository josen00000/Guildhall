#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine/Core/XmlUtils.hpp"

struct Vec3;

typedef std::map< std::string, std::string> StringMap;
typedef std::map< std::string, std::string>::iterator StringMapIterator;
typedef std::map< std::string, std::string>::const_iterator ConstStringMapIterator;

class NamedStrings {

public:
	void			PopulateFromXmlElementAttributes( const XmlElement& element );
	bool			PopulateFromXmlFile( const char* xmlFilePath);
	void			SetValue( const std::string& keyName, const std::string& newValue );

	bool			GetValue( const std::string& keyName, bool defaultValue ) const;
	int				GetValue( const std::string& keyName, int defaultValue ) const;
	float			GetValue( const std::string& keyName, float defaultValue ) const;
	std::string		GetValue( const std::string& keyName, std::string defaultValue ) const;
	std::string		GetValue( const std::string& keyName, const char* defaultValue ) const;
	Rgba8			GetValue( const std::string& keyName, const Rgba8& defaultValue ) const;
	Vec2			GetValue( const std::string& keyName, const Vec2& defaultValue ) const;
	Vec3			GetValue( const std::string& keyName, const Vec3& defaultValue ) const;
	IntVec2			GetValue( const std::string& keyName, const IntVec2& defaultValue ) const;
	
private:
	StringMap m_keyValuePairs;
};