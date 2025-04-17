/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef TOKEN_STREAM_H
#define TOKEN_STREAM_H

#include <vector>
#include "parserlib/token.h"

/**
 * @ingroup parserlib
 * @brief Parser token stream
 */
class CParserTokenStream
{
public:
	/**
	 * @brief Tokens array type
	 */
	typedef std::vector<parserToken_t>		tokens_t;

	/**
	 * @brief Constructor
	 */
	CParserTokenStream();

	/**
	 * @brief Constructor
	 * @param other		Other a token stream
	 */
	CParserTokenStream( const CParserTokenStream& other );

	/**
	 * @brief Extract body tokens
	 * @param stream			Output stream where will be placed tokens
	 * @param openBodyToken		Open body token
	 * @param closeBodyToken	Close body token
	 * @return Return TRUE if body tokens has been successfully extracted, otherwise FALSE
	 */
	bool ExtractBodyTokens( CParserTokenStream& stream, achar openBodyToken = '{', achar closeBodyToken = '}' );

	/**
	 * @brief Extract initialize tokens
	 * @param stream			Output stream where will be placed initialize code
	 * @param initializeToken	Initialize token
	 * @param delimiterToken	Delimiter token
	 * @return Return TRUE if initialize tokens has been successfully extracted, otherwise returns FALSE
	 */
	bool ExtractInitTokens( CParserTokenStream& stream, achar initializeToken = '=', achar delimiterToken = ';' );

	/**
	 * @brief Push token at the end of the stream
	 * @param pTokenString		Token string
	 * @param tokenStringSize	Token string size
	 * @param tokenID			Token ID
	 * @param scopeLevel		Scope level (bracket level)
	 * @param context			Token context
	 */
	FORCEINLINE void PushToken( const achar* pTokenString, uint32 tokenStringSize, int32 tokenID, int32 scopeLevel, const parserFileContext_t& context )
	{
		tokens.emplace_back( pTokenString, tokenStringSize, tokenID, scopeLevel, context );
	}

	/**
	 * @brief Push token at the end of the stream
	 * @param pTokenString		Token string
	 * @param tokenID			Token ID
	 * @param scopeLevel		Scope level (bracket level)
	 * @param context			Token context
	 */
	FORCEINLINE void PushToken( const achar* pTokenString, int32 tokenID, int32 scopeLevel, const parserFileContext_t& context )
	{
		tokens.emplace_back( pTokenString, tokenID, scopeLevel, context );
	}

	/**
	 * @brief Push token at the end of the stream
	 * @param token		Token
	 */
	FORCEINLINE void PushToken( parserToken_t&& token )
	{
		tokens.emplace_back( std::move( token ) );
	}

	/**
	 * @brief Pop token
	 * @param token		Retrieved token
	 * @return Return TRUE if token has been retrieved, otherwise FALSE (e.g no more tokens)
	 */
	bool PopToken( parserToken_t& token );

	/**
	 * @brief Get read token
	 * @return Return reference to token for read 
	 */
	FORCEINLINE parserToken_t& GetReadToken()
	{
		AssertMsg( !IsEmpty() || !IsEndOfStream(), "No more tokens to read" );
		return tokens[readPosition];
	}

	/**
	 * @brief Get read token
	 * @return Return reference to token for read
	 */
	FORCEINLINE const parserToken_t& GetReadToken() const
	{
		AssertMsg( !IsEmpty() || !IsEndOfStream(), "No more tokens to read" );
		return tokens[readPosition];
	}

	/**
	 * @brief Get last token in the stream
	 * @return Return reference to the last token for read
	 */
	FORCEINLINE const parserToken_t& GetLastToken() const
	{
		AssertMsg( !IsEmpty(), "No more tokens to read" );
		return tokens[tokens.size() - 1];
	}

	/**
	 * @brief Increment read position
	 */
	FORCEINLINE void IncrementReadPosition()
	{
		++readPosition;
	}

	/**
	 * @brief Get array of raw tokens
	 * @return Return array of raw tokens
	 */
	FORCEINLINE const tokens_t& GetTokens() const 
	{ 
		return tokens; 
	}

	/**
	 * @brief Get array of raw tokens
	 * @return Return array of raw tokens
	 */
	FORCEINLINE tokens_t& GetTokens() 
	{ 
		return tokens; 
	}

	/**
	 * @brief Is end of stream
	 * @return Return TRUE if we at the end of stream, otherwise FALSE
	 */
	FORCEINLINE bool IsEndOfStream() const 
	{ 
		return readPosition >= tokens.size();
	}

	/**
	 * @brief Is empty stream
	 * @return Return TRUE if the stream is empty, otherwise FALSE
	 */
	FORCEINLINE bool IsEmpty() const 
	{ 
		return tokens.empty();
	}

private:
	tokens_t	tokens;			/**< Tokens array */
	uint32		readPosition;	/**< Read position */
};

#endif // !TOKEN_STREAM_H