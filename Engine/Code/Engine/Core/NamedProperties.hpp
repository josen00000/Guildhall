#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine/ThirdParty/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"

struct Vec3;

class TypedPropertyBase {
public:
	virtual ~TypedPropertyBase(){}
};

template <typename T>
class TypedProperty : public TypedPropertyBase {
public:
	TypedProperty(){}
public:
	std::string m_key;
	T m_value;
};

class NamedProperties {
public:
	~NamedProperties();

public:
	bool PopulateFromXMLAttributes( const XmlElement& element );
	bool PopulateFromKeyValuePairs( std::string const& str );

	void SetValue( std::string const& keyName, std::string const& value );
	std::string GetValue( std::string const& keyName, std::string const& defValue );

	template <typename T>
	void SetValue( const std::string& keyName, const T& value );
	template<typename T>
	T GetValue( std::string const& keyName, T const& defValue ) const;

private:
	std::map<std::string, TypedPropertyBase*> m_keyValuePairs;
	NamedStrings m_namedStrings;
};

template <typename T>
void NamedProperties::SetValue( const std::string& keyName, const T& value )
{
	TypedProperty<T>* prop = new TypedProperty<T>();
	prop->m_value = value;
	
	m_keyValuePairs[keyName] = prop;
}

template<typename T>
T NamedProperties::GetValue( std::string const& keyName, T const& defValue ) const
{
	TypedPropertyBase* base = nullptr;
	auto iter = m_keyValuePairs.find( keyName );
	if( iter != m_keyValuePairs.end() ) {
		base = iter->second;
		TypedProperty<T>* prop = dynamic_cast<TypedProperty<T>*>( base );
		
		if( prop != nullptr ) {
			return prop->m_value;
		}
	}

	return defValue;
}

