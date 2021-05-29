#include "NamedProperties.hpp"
#include "Engine/Core/EngineCommon.hpp"

NamedProperties::~NamedProperties()
{
	for( auto iter = m_keyValuePairs.begin(); iter != m_keyValuePairs.end(); ++iter ) {
		SELF_SAFE_RELEASE( iter->second );
	}

	m_keyValuePairs.clear();
}

bool NamedProperties::PopulateFromXMLAttributes( const XmlElement& element )
{
	UNUSED(element);
	return true;
}

bool NamedProperties::PopulateFromKeyValuePairs( std::string const& str )
{
	UNUSED(str);
	return true;
}

void NamedProperties::SetValue( std::string const& keyName, std::string const& value )
{
	TypedProperty<std::string>* prop = new TypedProperty<std::string>();
	prop->m_value = value;

	m_keyValuePairs[keyName] = prop;
}

std::string NamedProperties::GetValue( std::string const& keyName, std::string const& defValue )
{
	TypedPropertyBase* base = nullptr;
	auto iter = m_keyValuePairs.find( keyName );
	if( iter != m_keyValuePairs.end() ) {
		base = iter->second;
		TypedProperty<std::string>* prop = dynamic_cast<TypedProperty<std::string>*>(base);
		if( prop != nullptr ) {
			return prop->m_value;
		}
	}
	return defValue;
}


