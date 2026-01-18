#pragma once
#include "parserlib/filecontext.h"

//-----------------------------------------------------------------------------
// Parsed file token
//-----------------------------------------------------------------------------
struct parserToken_t
{
	parserToken_t()
		: tokenID( 0 )
		, scopeLevel( -1 )
	{
	}

	parserToken_t( const char* pTokenString, uint32 tokenStringSize, int32 tokenID, int32 scopeLevel, const parserFileContext_t& context )
		: tokenString( pTokenString, tokenStringSize )
		, tokenID( tokenID )
		, scopeLevel( scopeLevel )
		, context( context )
	{
	}

	parserToken_t( const char* pTokenString, int32 tokenID, int32 scopeLevel, const parserFileContext_t& context )
		: tokenString( pTokenString )
		, tokenID( tokenID )
		, scopeLevel( scopeLevel )
		, context( context )
	{
	}

	parserToken_t( const parserToken_t& other )
		: tokenString( other.tokenString )
		, tokenID( other.tokenID )
		, scopeLevel( other.scopeLevel )
		, context( other.context )
	{
	}

	parserToken_t( parserToken_t&& other )
		: tokenString( std::move( other.tokenString ) )
		, tokenID( std::move( other.tokenID ) )
		, scopeLevel( std::move( other.scopeLevel ) )
		, context( std::move( other.context ) )
	{
	}

	parserToken_t& operator=( const parserToken_t& other );
	std::string	   ToString() const;

	std::string			tokenString;
	int32				tokenID;
	int32				scopeLevel;	 // Scope level (bracket level)
	parserFileContext_t context;
};

#include "parserlib/token.inl"