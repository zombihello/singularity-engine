#pragma once

/*
==================
CParserTokenStream::PushToken
==================
*/
FORCEINLINE void CParserTokenStream::PushToken( const char* pTokenString, uint32 tokenStringSize, int32 tokenID, int32 scopeLevel, const parserFileContext_t& context )
{
	tokens.emplace_back( pTokenString, tokenStringSize, tokenID, scopeLevel, context );
}

/*
==================
CParserTokenStream::PushToken
==================
*/
FORCEINLINE void CParserTokenStream::PushToken( const char* pTokenString, int32 tokenID, int32 scopeLevel, const parserFileContext_t& context )
{
	tokens.emplace_back( pTokenString, tokenID, scopeLevel, context );
}

/*
==================
CParserTokenStream::PushToken
==================
*/
FORCEINLINE void CParserTokenStream::PushToken( parserToken_t&& token )
{
	tokens.emplace_back( std::move( token ) );
}

/*
==================
CParserTokenStream::GetReadToken
==================
*/
FORCEINLINE parserToken_t& CParserTokenStream::GetReadToken()
{
	AssertMsg( !IsEmpty() || !IsEndOfStream(), "No more tokens to read" );
	return tokens[readPosition];
}

/*
==================
CParserTokenStream::GetReadToken
==================
*/
FORCEINLINE const parserToken_t& CParserTokenStream::GetReadToken() const
{
	AssertMsg( !IsEmpty() || !IsEndOfStream(), "No more tokens to read" );
	return tokens[readPosition];
}

/*
==================
CParserTokenStream::GetLastToken
==================
*/
FORCEINLINE const parserToken_t& CParserTokenStream::GetLastToken() const
{
	AssertMsg( !IsEmpty(), "No more tokens to read" );
	return tokens[tokens.size() - 1];
}

/*
==================
CParserTokenStream::IncrementReadPosition
==================
*/
FORCEINLINE void CParserTokenStream::IncrementReadPosition()
{
	++readPosition;
}

/*
==================
CParserTokenStream::GetTokens
==================
*/
FORCEINLINE const CParserTokenStream::tokens_t& CParserTokenStream::GetTokens() const
{
	return tokens;
}

/*
==================
CParserTokenStream::GetTokens
==================
*/
FORCEINLINE CParserTokenStream::tokens_t& CParserTokenStream::GetTokens()
{
	return tokens;
}

/*
==================
CParserTokenStream::IsEndOfStream
==================
*/
FORCEINLINE bool CParserTokenStream::IsEndOfStream() const
{
	return readPosition >= tokens.size();
}

/*
==================
CParserTokenStream::IsEmpty
==================
*/
FORCEINLINE bool CParserTokenStream::IsEmpty() const
{
	return tokens.empty();
}
