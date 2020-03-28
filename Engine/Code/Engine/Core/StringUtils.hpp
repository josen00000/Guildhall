#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

typedef std::vector<std::string> Strings;

//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
std::string Stringv( const char* format, va_list args );
Strings SplitStringOnDelimiter( const std::string& originalString, const char delimiterToSplitOn, int splitNum = 0 );
Strings SplitStringOnDelimiter( const std::string& originalString, const std::string& delimiterToSplitOn, int splitNum = 0 );
std::string GetStringWithoutSpace( const char* originalString );
std::string GetStringWithoutSpace( std::string originalString );
bool GetBoolFromText( const char* text );
std::string GetStringFromBool( bool input ); 

