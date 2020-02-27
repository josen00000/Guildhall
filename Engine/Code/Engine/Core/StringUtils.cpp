#include "StringUtils.hpp"
#include <stdarg.h>
#include <algorithm>
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

Strings SplitStringOnDelimiter( const std::string& originalString, const std::string& delimiterToSplitOn )
{
	Strings resultStrings;
	size_t splitStartIndex = 0;

	while(true){
		size_t splitEndIndex = originalString.find(delimiterToSplitOn, splitStartIndex);
		size_t splitLength = splitEndIndex - splitStartIndex; 
		std::string subString = std::string(originalString, splitStartIndex, splitLength);
		resultStrings.push_back(subString);

		if( splitEndIndex == std::string::npos){ break;}
		splitStartIndex = splitEndIndex + delimiterToSplitOn.size();
	}
	return resultStrings;

}


Strings SplitStringOnDelimiter( const std::string& originalString, const char delimiterToSplitOn )
{
	Strings resultStrings;
	size_t splitStartIndex = 0;

	while( true ) {
		size_t splitEndIndex = originalString.find( delimiterToSplitOn, splitStartIndex );
		size_t splitLength = splitEndIndex - splitStartIndex;
		std::string subString = std::string( originalString, splitStartIndex, splitLength );
		resultStrings.push_back( subString );

		if( splitEndIndex == std::string::npos ) { break; }
		splitStartIndex = splitEndIndex + 1;
	}
	return resultStrings;
}

std::string GetStringWithoutSpace( const char* originalString )
{
	size_t startIndex = 0;
	while( true ) {
		if( originalString[startIndex] != ' ' ){ break;}
		startIndex++;
	}
	
	size_t endIndex = startIndex;
	while(true){
		if( originalString[endIndex] == '\0' || originalString[endIndex] == ' '){
			break;
		}
		if( originalString[endIndex] != ' ' ){
			endIndex++;
		}
	}
	size_t stringLength = endIndex - startIndex;
	std::string result = std::string(originalString, startIndex, stringLength );
	return result;
}

bool GetBoolFromText( const char* text )
{

	//return false might not because of the value
	std::string value = GetStringWithoutSpace( text );
	for(std::string::iterator index = value.begin(); index < value.end(); ++ index){
		*index = (char) tolower(*index);
	}

	if(value.size() == 1){
		if(value.compare("t") == 0){
			return true;
		}
		if(value.compare( "f" ) == 0){
			return false;
		}
	}
	else if(value.size() == 4){
		if(value.compare("true") == 0){
			return true;
		}
	}
	else if(value.size() == 5){
		if(value.compare("false") == 0){
			return false;
		}
	}
	else{
		return false;
	}
	ERROR_AND_DIE(" bool format error");
}

std::string GetStringFromBool( bool input )
{
	std::string result;
	if( input ) {
		result = "true";
	}
	else {
		result = "false";
	}
	return result;
}

