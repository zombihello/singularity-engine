#include "pch_parserlib.h"
#include "parserlib/lexer_emitter.h"

/*
==================
CParserLexerEmitter::CParserLexerEmitter
==================
*/
CParserLexerEmitter::CParserLexerEmitter( CParserTokenEater& parserListener, const achar* pPath )
	: parserListener( parserListener )
	, path( pPath )
{}

/*
==================
CParserLexerEmitter::Comment
==================
*/
void CParserLexerEmitter::Comment( const parserLexerState_t& lexerState )
{
	uint32			line				= lexerState.tokenStart.line + 1;
	uint32			scopeLevel			= lexerState.scopeLevel;
	uint32			charLineStart		= lexerState.sequenceLineStart.charOffset;
	uint32			charStartPosition	= lexerState.sequenceStart.charOffset;
	uint32			charEndPosition		= lexerState.currentContext.charOffset;

	// Sequences include the surrounding quote marks, so we need to reduce the size of the token text respectively
	const achar*	pText				= lexerState.pSource + lexerState.sequenceStart.byteOffset + 1;
	uint32			size				= lexerState.currentContext.byteOffset - lexerState.sequenceStart.byteOffset - 2;
	parserListener.OnEmitComment( line, pText, size, scopeLevel, charLineStart, charStartPosition, charEndPosition );
}

/*
==================
CParserLexerEmitter::Token
==================
*/
void CParserLexerEmitter::Token( const parserLexerState_t& lexerState, uint32 tokenID )
{
	uint32			line				= lexerState.tokenStart.line + 1;
	uint32			scopeLevel			= lexerState.scopeLevel;
	uint32			charLineStart		= lexerState.lineStart.charOffset;
	uint32			charStartPosition	= lexerState.tokenStart.charOffset;
	uint32			charEndPosition		= lexerState.currentContext.charOffset;
	const achar*	pText				= lexerState.pSource + lexerState.tokenStart.byteOffset;
	uint32			size				= lexerState.currentContext.byteOffset - lexerState.tokenStart.byteOffset;
	parserListener.OnEmitToken( line, tokenID, pText, size, scopeLevel, charLineStart, charStartPosition, charEndPosition );
}

/*
==================
CParserLexerEmitter::Sequence
==================
*/
void CParserLexerEmitter::Sequence( const parserLexerState_t& lexerState, uint32 tokenID )
{
	uint32			line				= lexerState.tokenStart.line + 1;
	uint32			scopeLevel			= lexerState.scopeLevel;
	uint32			charLineStart		= lexerState.sequenceLineStart.charOffset;
	uint32			charStartPosition	= lexerState.sequenceStart.charOffset;
	uint32			charEndPosition		= lexerState.currentContext.charOffset;

	// Sequences include the surrounding quote marks, so we need to reduce the size of the token text respectively
	const achar*	pText				= lexerState.pSource + lexerState.sequenceStart.byteOffset + 1;
	uint32			size				= lexerState.currentContext.byteOffset - lexerState.sequenceStart.byteOffset - 2;
	parserListener.OnEmitToken( line, tokenID, pText, size, scopeLevel, charLineStart, charStartPosition, charEndPosition );
}

/*
==================
CParserLexerEmitter::Error
==================
*/
void CParserLexerEmitter::Error( const parserLexerState_t& lexerState, const achar* pMessage )
{
	uint32			line				= lexerState.tokenStart.line + 1;
	uint32			charLineStart		= lexerState.lineStart.charOffset;
	uint32			charStartPosition	= lexerState.tokenStart.charOffset;
	uint32			charEndPosition		= lexerState.currentContext.charOffset;
	const achar*	pText				= lexerState.pSource + lexerState.tokenStart.byteOffset;
	uint32			size				= lexerState.currentContext.byteOffset - lexerState.tokenStart.byteOffset;

	parserFileContext_t		context( path, line, charLineStart, charStartPosition, charEndPosition );
	::Error( "%s: %s", context.ToString().c_str(), pMessage );
}