/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef TOKEN_H
#define TOKEN_H

#include "parserlib/filecontext.h"

/**
 * @ingroup parserlib
 * @brief Parsed file token
 */
struct parserToken_t
{
	/**
	 * @brief Constructor
	 */
	parserToken_t()
		: tokenID( 0 )
		, scopeLevel( -1 )
	{}

	/**
	 * @brief Constructor
	 * @param pTokenString		Token string
	 * @param tokenStringSize	Token string size
	 * @param tokenID			Token ID
	 * @param scopeLevel		Scope level (bracket level)
	 * @param context			Token context
	 */
	parserToken_t( const achar* pTokenString, uint32 tokenStringSize, int32 tokenID, int32 scopeLevel, const parserFileContext_t& context )
		: tokenString( pTokenString, tokenStringSize )
		, tokenID( tokenID )
		, scopeLevel( scopeLevel )
		, context( context )
	{}

	/**
	 * @brief Constructor
	 * @param pTokenString		Token string
	 * @param tokenID			Token ID
	 * @param scopeLevel		Scope level (bracket level)
	 * @param context			Token context
	 */
	parserToken_t( const achar* pTokenString, int32 tokenID, int32 scopeLevel, const parserFileContext_t& context )
		: tokenString( pTokenString )
		, tokenID( tokenID )
		, scopeLevel( scopeLevel )
		, context( context )
	{}

	/**
	 * @brief Constructor
	 * @param other		Other parser token
	 */
	parserToken_t( const parserToken_t& other )
		: tokenString( other.tokenString )
		, tokenID( other.tokenID )
		, scopeLevel( other.scopeLevel )
		, context( other.context )
	{}

	/**
	 * @brief Constructor
	 * @param other		Other parser token
	 */
	parserToken_t( parserToken_t&& other )
		: tokenString( std::move( other.tokenString ) )
		, tokenID( std::move( other.tokenID ) )
		, scopeLevel( std::move( other.scopeLevel ) )
		, context( std::move( other.context ) )
	{}

	/**
	 * @brief Copy operator
	 * @param other		Other parser token
	 * @return Return reference to self
	 */
	FORCEINLINE parserToken_t& operator=( const parserToken_t& other )
	{
		tokenString = other.tokenString;
		tokenID		= other.tokenID;
		scopeLevel	= other.scopeLevel;
		context		= other.context;
		return *this;
	}

	/**
	 * @brief Convert the token to string
	 * @return Return converted the token to string
	 */
	FORCEINLINE std::string ToString() const
	{
		return S_Sprintf( "Token %i \"%hs\", #%i at %hs", tokenID, !tokenString.empty() ? tokenString.data() : "<TEXT_EMPTY>", scopeLevel, context.ToString().c_str() );
	}

	std::string				tokenString;	/**< Token string */
	int32					tokenID;		/**< Token ID */
	int32					scopeLevel;		/**< Scope level (bracket level) */
	parserFileContext_t		context;		/**< Token context */
};

#endif // !TOKEN_H