#pragma once

/*
==================
parserLexerState_t::NextLine
==================
*/
FORCEINLINE void parserLexerState_t::NextLine()
{
	++currentContext.line;
	lineStart = currentContext;
}

/*
==================
parserLexerState_t::StoreTokenStart
==================
*/
FORCEINLINE void parserLexerState_t::StoreTokenStart()
{
	tokenStart = currentContext;
}

/*
==================
parserLexerState_t::StoreSequenceStart
==================
*/
FORCEINLINE void parserLexerState_t::StoreSequenceStart( uint32 charactersToSkip /*= 0*/ )
{
	sequenceStart = tokenStart;
	if ( charactersToSkip )
	{
		sequenceStart.byteOffset += charactersToSkip;
		sequenceStart.charOffset += charactersToSkip;
	}

	sequenceLineStart = lineStart;
}

/*
==================
parserLexerState_t::UpdateContext
==================
*/
FORCEINLINE void parserLexerState_t::UpdateContext( const achar* pToken, uint32 tokenByteLength )
{
	currentContext.byteOffset += tokenByteLength;
	currentContext.charOffset += Parserlib_GetNumChars( pToken, tokenByteLength );
}

/*
==================
parserLexerState_t::UpdateContext
==================
*/
FORCEINLINE void parserLexerState_t::EmitToken( uint32 tokenID )
{
	pLexerListener->Token( *this, tokenID );
}

/*
==================
parserLexerState_t::EmitSequence
==================
*/
FORCEINLINE void parserLexerState_t::EmitSequence( uint32 tokenID )
{
	pLexerListener->Sequence( *this, tokenID );
}

/*
==================
parserLexerState_t::EmitComment
==================
*/
FORCEINLINE void parserLexerState_t::EmitComment()
{
	pLexerListener->Comment( *this );
}

/*
==================
parserLexerState_t::EmitError
==================
*/
FORCEINLINE void parserLexerState_t::EmitError( const achar* pMessage )
{
	pLexerListener->Error( *this, pMessage );
}
