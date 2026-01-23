#include "pch_parserlib.h"
#include "stdlib/filetools.h"
#include "utils/parserlib/lexerstate.h"
#include "utils/parserlib/lexerlistener.h"

/*
==================
CParserLexerListener::CParserLexerListener
==================
*/
CParserLexerListener::CParserLexerListener( CParserTokenEater& parserListener, const char* pPath )
	: bHasError( false )
	, parserListener( parserListener )
	, path( pPath )
{
	if ( !S_IsAbsolutePath( pPath ) )
	{
		S_MakeAbsolutePath( pPath, path );
	}
	S_RemoveDotPathSeparators( path );
}

/*
==================
CParserLexerListener::Comment
==================
*/
void CParserLexerListener::Comment( const parserLexerState_t& lexerState )
{
	uint32 line				 = lexerState.tokenStart.line + 1;
	uint32 scopeLevel		 = lexerState.scopeLevel;
	uint32 charLineStart	 = lexerState.sequenceLineStart.charOffset;
	uint32 charStartPosition = lexerState.sequenceStart.charOffset;
	uint32 charEndPosition	 = lexerState.currentContext.charOffset;

	// Sequences include the surrounding quote marks, so we need to reduce the size of the token text respectively
	const char* pText = lexerState.pSource + lexerState.sequenceStart.byteOffset + 1;
	uint32		 size  = lexerState.currentContext.byteOffset - lexerState.sequenceStart.byteOffset - 2;
	parserListener.OnEmitComment( line, pText, size, scopeLevel, charLineStart, charStartPosition, charEndPosition );
}

/*
==================
CParserLexerListener::Token
==================
*/
void CParserLexerListener::Token( const parserLexerState_t& lexerState, uint32 tokenID )
{
	uint32		 line			   = lexerState.tokenStart.line + 1;
	uint32		 scopeLevel		   = lexerState.scopeLevel;
	uint32		 charLineStart	   = lexerState.lineStart.charOffset;
	uint32		 charStartPosition = lexerState.tokenStart.charOffset;
	uint32		 charEndPosition   = lexerState.currentContext.charOffset;
	const char* pText			   = lexerState.pSource + lexerState.tokenStart.byteOffset;
	uint32		 size			   = lexerState.currentContext.byteOffset - lexerState.tokenStart.byteOffset;
	parserListener.OnEmitToken( line, tokenID, pText, size, scopeLevel, charLineStart, charStartPosition, charEndPosition );
}

/*
==================
CParserLexerListener::Sequence
==================
*/
void CParserLexerListener::Sequence( const parserLexerState_t& lexerState, uint32 tokenID )
{
	uint32 line				 = lexerState.tokenStart.line + 1;
	uint32 scopeLevel		 = lexerState.scopeLevel;
	uint32 charLineStart	 = lexerState.sequenceLineStart.charOffset;
	uint32 charStartPosition = lexerState.sequenceStart.charOffset;
	uint32 charEndPosition	 = lexerState.currentContext.charOffset;

	// Sequences include the surrounding quote marks, so we need to reduce the size of the token text respectively
	const char* pText = lexerState.pSource + lexerState.sequenceStart.byteOffset + 1;
	uint32		 size  = lexerState.currentContext.byteOffset - lexerState.sequenceStart.byteOffset - 2;
	parserListener.OnEmitToken( line, tokenID, pText, size, scopeLevel, charLineStart, charStartPosition, charEndPosition );
}

/*
==================
CParserLexerListener::Error
==================
*/
void CParserLexerListener::Error( const parserLexerState_t& lexerState, const char* pMessage )
{
	uint32		 line			   = lexerState.tokenStart.line + 1;
	uint32		 charLineStart	   = lexerState.lineStart.charOffset;
	uint32		 charStartPosition = lexerState.tokenStart.charOffset;
	uint32		 charEndPosition   = lexerState.currentContext.charOffset;
	const char* pText			   = lexerState.pSource + lexerState.tokenStart.byteOffset;
	uint32		 size			   = lexerState.currentContext.byteOffset - lexerState.tokenStart.byteOffset;

	parserFileContext_t context( path, line, charLineStart, charStartPosition, charEndPosition );
	::Error( "%s: %s", context.ToString().c_str(), pMessage );
	bHasError = true;
}