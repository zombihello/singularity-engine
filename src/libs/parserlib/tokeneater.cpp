#include "pch_parserlib.h"
#include "stdlib/filetools.h"
#include "parserlib/tokeneater.h"

/*
==================
CParserTokenEater::CParserTokenEater
==================
*/
CParserTokenEater::CParserTokenEater( CParserTokenStream& tokens, const achar* pPath )
	: tokens( tokens )
	, path( pPath )
{
	if ( !S_IsAbsolutePath( pPath ) )
	{
		S_MakeAbsolutePath( pPath, path );
	}
	S_RemoveDotPathSeparators( path );
	tokens.GetTokens().reserve( 1000 );
}

/*
==================
CParserTokenEater::OnEmitToken
==================
*/
void CParserTokenEater::OnEmitToken( uint32 line, uint32 tokenID, const achar* pTokenString, uint32 tokenStringSize, uint32 scopeLevel, uint32 charLineStart, uint32 charStart, uint32 charEnd )
{
	// Setup context information
	parserFileContext_t		context( path, line, charLineStart, charStart, charEnd );
	tokens.PushToken( std::forward<parserToken_t>( parserToken_t( pTokenString, tokenStringSize, tokenID, scopeLevel, context ) ) );
}

/*
==================
CParserTokenEater::OnEmitComment
==================
*/
void CParserTokenEater::OnEmitComment( uint32 line, const achar* pText, uint32 textSize, uint32 scopeLevel, uint32 charLineStart, uint32 charStart, uint32 charEnd )
{}

/*
==================
CParserTokenEater::OnEmitError
==================
*/
void CParserTokenEater::OnEmitError( uint32 line, const achar* pMessage )
{
	Error( "%s: %s", parserFileContext_t( path, line ).ToString().c_str(), pMessage );
}