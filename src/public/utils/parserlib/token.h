#pragma once
#include "utils/parserlib/filecontext.h"

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
		: tokenString( eastl::move( other.tokenString ) )
		, tokenID( eastl::move( other.tokenID ) )
		, scopeLevel( eastl::move( other.scopeLevel ) )
		, context( eastl::move( other.context ) )
	{
	}

	parserToken_t& operator=( const parserToken_t& other );
	eastl::string  ToString() const;

	eastl::string		tokenString;
	int32				tokenID;
	int32				scopeLevel;	 // Scope level (bracket level)
	parserFileContext_t context;
};

#include "utils/parserlib/token.inl"
