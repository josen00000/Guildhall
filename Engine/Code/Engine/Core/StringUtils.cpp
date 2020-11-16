#include "StringUtils.hpp"
#include <stdarg.h>
#include <algorithm>
#include <io.h>
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

std::string Stringv( const char* format, va_list args )
{
	char buffer[1024];
	vsnprintf_s( buffer, 1024, format, args );
	return buffer;
}

Strings SplitStringOnDelimiter( const std::string& originalString, const std::string& delimiterToSplitOn, int splitNum )
{
	Strings resultStrings;
	size_t splitStartIndex = 0;
	int resultStringsNumber = 0;

	while(true){
		size_t splitEndIndex = originalString.find(delimiterToSplitOn, splitStartIndex);
		size_t splitLength = splitEndIndex - splitStartIndex; 
		std::string subString = std::string(originalString, splitStartIndex, splitLength);
		resultStrings.push_back(subString);
		resultStringsNumber++;

		if( resultStringsNumber == splitNum ) {
			std::string subString1 = std::string( originalString, splitEndIndex + delimiterToSplitOn.size(), std::string::npos );
			resultStrings.push_back( subString1 );
			break;
		}

		if( splitEndIndex == std::string::npos){ break;}
		splitStartIndex = splitEndIndex + delimiterToSplitOn.size();
	}
	return resultStrings;
}


Strings SplitStringOnDelimiter( const std::string& originalString, const char delimiterToSplitOn, int splitNum )
{
	Strings resultStrings;
	size_t splitStartIndex = 0;
	int resultStringsNumber = 0;

	while( true ) {
		size_t splitEndIndex = originalString.find( delimiterToSplitOn, splitStartIndex );

		if( splitEndIndex == std::string::npos || resultStringsNumber == splitNum ) {
			std::string subString1 = std::string( originalString, splitStartIndex, std::string::npos );
			resultStrings.push_back( subString1 );
			break;
		}
		int delimiterLength = 0;
 		while( originalString[splitEndIndex] == delimiterToSplitOn ){
 			splitEndIndex++;
			delimiterLength++;
 		}
		
		size_t splitLength = splitEndIndex - splitStartIndex - delimiterLength;
		std::string subString = std::string( originalString, splitStartIndex, splitLength );
		resultStrings.push_back( subString );
		resultStringsNumber++;
		splitStartIndex = splitEndIndex;
		if( splitEndIndex >= originalString.size() ){ break; }
	}

	return resultStrings;
}

std::string GetStringWithoutSpace( const char* originalString )
{
	size_t startIndex = 0;
	while( true ) {
		if( originalString[startIndex] != ' ' ){ break; }
		startIndex++;
	}
	
	size_t endIndex = startIndex;
	while( true ){
		if( originalString[endIndex] == '\0' || originalString[endIndex] == ' '){
			break;
		}
		if( originalString[endIndex] != ' ' ){
			endIndex++;
		}
	}
	size_t stringLength = endIndex - startIndex;
	std::string result = std::string( originalString, startIndex, stringLength );
	return result;
}

std::string GetStringWithoutSpace( std::string originalString )
{
	return GetStringWithoutSpace( originalString.c_str() );
}

std::string TrimStringWithOneSpace( std::string originalString )
{
	int copyIndex = 0;
	int spaceNum = 0;
	std::string result;
	result.resize( originalString.size() );
	for( int i = 0; i < originalString.size(); i++ ) {
		if( originalString[i] != ' ' ) {
			result[copyIndex] = originalString[i];
			copyIndex++;
			spaceNum = 0;
		}
		else {
			if( spaceNum == 0 ) {
				result[copyIndex] = originalString[i];
				copyIndex++;
				spaceNum++;
			}
			else {
				continue;
			}
		}
	}
	return result;
}

Strings GetStringsWithoutSpace( std::string originalString )
{
	return SplitStringOnDelimiter( originalString, ' ' );
}

std::string GetRidOfAllSpaceOfString( std::string originalString )
{
	std::string result = originalString;
	int resultIndex = 0;
	for( int i = 0; i < originalString.size(); i++ ) {
		if( originalString[i] == '\0' || originalString[i] == ' '){
			continue;
		}
		result[resultIndex] = originalString[i];
		resultIndex++;
	}
	if( result.size() > resultIndex ) {
		result.erase( resultIndex );
	}

	return result;
}

Strings GetFileNamesInFolder( const std::string& folderPath, const char* filePattern )
{
	Strings fileNamesInFolder;
	std::string fileNamePattern = filePattern ? filePattern : "*";
	std::string filePath = folderPath + "/" + fileNamePattern;
	_finddata_t fileInfo;
	intptr_t searchHandle = _findfirst( filePath.c_str(), &fileInfo );
	while( searchHandle != -1 ) {
		fileNamesInFolder.push_back( fileInfo.name );
		int errorCode = _findnext( searchHandle, &fileInfo );
		if( errorCode != 0 ) {
			break;
		}
	}

	return fileNamesInFolder;
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

bool IsStringFullOfSpace( std::string text )
{
	return text.find_first_not_of( ' ' ) == std::string::npos ;
}

bool IsStringStartWithChar( std::string text, const char c )
{
	return text[0] == c;
}

