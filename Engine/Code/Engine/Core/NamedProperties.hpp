#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine/ThirdParty/XmlUtils.hpp"

struct Vec3;

class NamedProperties {
public:
	bool PopulateFromXMLAttributes( const XmlElement& element );
	bool PopulateFromKeyValuePairs( std::string const& str );

	void SetValue( std::string const& keyName, std::string const& defValue );

	template<typename T>
	T GetValue( std::string const& keyName, T const& defValue ) const;

private:


};