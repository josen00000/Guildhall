#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

typedef std::vector<std::string> Strings;

//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
std::string Stringv( const char* format, va_list args );
Strings SplitStringOnDelimiter( const std::string& originalString, const char delimiterToSplitOn, int splitNum = -1 ); // splitnum = -1 find all substring. 1 return two sub
Strings SplitStringOnDelimiter( const std::string& originalString, const std::string& delimiterToSplitOn, int splitNum = -1 );

std::string GetStringWithoutSpace( const char* originalString );
std::string GetStringWithoutSpace( std::string originalString );
std::string TrimStringWithOneSpace( std::string originalString );
Strings GetStringsWithoutSpace( std::string originalString );

Strings GetFileNamesInFolder( const std::string& folderPath, const char* filePattern );

bool GetBoolFromText( const char* text );
std::string GetStringFromBool( bool input );

// helper function
bool IsStringFullOfSpace( std::string text );
bool IsStringStartWithChar( std::string text, const char c );
