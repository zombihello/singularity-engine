#pragma once
#include <vector>
#include "parserlib/token.h"

//-----------------------------------------------------------------------------
// Parser token stream
//-----------------------------------------------------------------------------
class CParserTokenStream
{
public:
	typedef std::vector<parserToken_t>		tokens_t;

	CParserTokenStream();
	CParserTokenStream( const CParserTokenStream& other );

	bool ExtractBodyTokens( CParserTokenStream& stream, achar openBodyToken = '{', achar closeBodyToken = '}' );
	bool ExtractInitTokens( CParserTokenStream& stream, achar initializeToken = '=', achar delimiterToken = ';' );
	
	void IncrementReadPosition();

	// Push token at the end of the stream and pop token
	void PushToken( const achar* pTokenString, uint32 tokenStringSize, int32 tokenID, int32 scopeLevel, const parserFileContext_t& context );
	void PushToken( const achar* pTokenString, int32 tokenID, int32 scopeLevel, const parserFileContext_t& context );
	void PushToken( parserToken_t&& token );
	bool PopToken( parserToken_t& token );

	bool IsEndOfStream() const;
	bool IsEmpty() const;

	parserToken_t& GetReadToken();
	const parserToken_t& GetReadToken() const;
	const parserToken_t& GetLastToken() const;
	const tokens_t& GetTokens() const;
	tokens_t& GetTokens();

private:
	tokens_t	tokens;
	uint32		readPosition;
};

#include "parserlib/tokenstream.inl"