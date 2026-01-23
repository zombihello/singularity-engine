#pragma once
#include "utils/parserlib/tokeneater.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
struct parserLexerState_t;

//-----------------------------------------------------------------------------
// Parser lexer listener interface
//
// You need to create a subclass of CParserLexerListener in
// order to receive the tokenised output of a lexer
//-----------------------------------------------------------------------------
class CParserLexerListener
{
public:
	CParserLexerListener( CParserTokenEater& parserListener, const char* pPath );

	// A comment has been encountered (could be either single or multiline)
	virtual void Comment( const parserLexerState_t& lexerState );

	// A standard token
	virtual void Token( const parserLexerState_t& lexerState, uint32 tokenID );

	// A token denoted by a start and end series of chars (such as a string or name)
	virtual void Sequence( const parserLexerState_t& lexerState, uint32 tokenID );

	// A section of script that has failed to match any token rules
	virtual void	 Error( const parserLexerState_t& lexerState, const char* pMessage );
	FORCEINLINE bool HasError() const
	{
		return bHasError;
	}

protected:
	bool			   bHasError;
	CParserTokenEater& parserListener;
	eastl::string	   path;
};
