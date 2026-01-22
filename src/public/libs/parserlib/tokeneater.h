#pragma once
#include "parserlib/tokenstream.h"
#include "parserlib/stringbuffer.h"

//-----------------------------------------------------------------------------
// Connection interface between lexer and a file parser
//-----------------------------------------------------------------------------
class CParserTokenEater
{
public:
	CParserTokenEater( CParserTokenStream& tokens, const char* pPath );

	virtual void OnEmitToken( uint32 line, uint32 tokenID, const char* pTokenString, uint32 tokenStringSize, uint32 scopeLevel, uint32 charLineStart, uint32 charStart, uint32 charEnd );
	virtual void OnEmitComment( uint32 line, const char* pText, uint32 textSize, uint32 scopeLevel, uint32 charLineStart, uint32 charStart, uint32 charEnd );
	virtual void OnEmitError( uint32 line, const char* pMessage );

protected:
	eastl::string			path;
	CParserTokenStream& tokens;	 // Here will be places all of parsed tokens
};