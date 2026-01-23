#pragma once
#include "tier1/defines.h"
#include "tier1/types.h"

//-----------------------------------------------------------------------------
// Parser lexer context
//-----------------------------------------------------------------------------
struct parserLexerContext_t
{
	parserLexerContext_t()
		: line( 0 )
		, byteOffset( 0 )
		, charOffset( 0 )
	{
	}

	parserLexerContext_t( const parserLexerContext_t& other )
		: line( other.line )
		, byteOffset( other.byteOffset )
		, charOffset( other.charOffset )
	{
	}

	uint32 line;		// The line that this context refers to
	uint32 byteOffset;	// The associated position in the raw input buffer
	uint32 charOffset;	// The associated position in the input buffer resolved as a UTF-8 string
};