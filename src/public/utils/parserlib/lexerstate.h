#pragma once
#include "utils/parserlib/lexercontext.h"
#include "utils/parserlib/lexerlistener.h"

//-----------------------------------------------------------------------------
// Used to manage the state of the lexer internally
//-----------------------------------------------------------------------------
struct parserLexerState_t
{
public:
	parserLexerState_t( const char* pSource, CParserLexerListener* pLexerListener )
		: pSource( pSource )
		, scopeLevel( 0 )
		, pLexerListener( pLexerListener )
	{
	}

	parserLexerState_t( const parserLexerState_t& other )
		: pSource( other.pSource )
		, currentContext( other.currentContext )
		, tokenStart( other.tokenStart )
		, lineStart( other.lineStart )
		, sequenceStart( other.sequenceStart )
		, sequenceLineStart( other.sequenceLineStart )
		, scopeLevel( other.scopeLevel )
		, pLexerListener( other.pLexerListener )
	{
	}

	void NextLine();
	void StoreTokenStart();
	void StoreSequenceStart( uint32 charactersToSkip = 0 );
	void UpdateContext( const char* pToken, uint32 tokenByteLength );

	void EmitToken( uint32 tokenID );
	void EmitSequence( uint32 tokenID );
	void EmitComment();
	void EmitError( const char* pMessage );

	const char*			 pSource;			 // The raw input buffer
	parserLexerContext_t currentContext;	 // The current context of the lexer. Usually this will be synonymous with the "end" of the token emitted
	parserLexerContext_t tokenStart;		 // The start of the token emitted
	parserLexerContext_t lineStart;			 // The context of the current line, at column 0
	parserLexerContext_t sequenceStart;		 // The start of the sequence emitted (a sequence being a token enclosed in a header/footer, such as a "string" or 'name')
	parserLexerContext_t sequenceLineStart;	 // The context of the line at the start of the sequence, at column 0
	uint32				 scopeLevel;		 // The number of opened scopes encountered up till now

private:
	CParserLexerListener* pLexerListener;
};

#include "utils/parserlib/lexerstate.inl"
