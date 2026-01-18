#include "pch_parserlib.h"
#include "parserlib/tokenstream.h"

/*
==================
CParserTokenStream::CParserTokenStream
==================
*/
CParserTokenStream::CParserTokenStream()
	: readPosition( 0 )
{
}

/*
==================
CParserTokenStream::CParserTokenStream
==================
*/
CParserTokenStream::CParserTokenStream( const CParserTokenStream& other )
	: tokens( other.tokens )
	, readPosition( other.readPosition )
{
}

/*
==================
CParserTokenStream::PopToken
==================
*/
bool CParserTokenStream::PopToken( parserToken_t& token )
{
	// Get token
	if ( readPosition < tokens.size() )
	{
		token = std::forward<parserToken_t>( tokens[readPosition] );
		++readPosition;
		return true;
	}

	// No more tokens
	return false;
}

/*
==================
CParserTokenStream::ExtractBodyTokens
==================
*/
bool CParserTokenStream::ExtractBodyTokens( CParserTokenStream& stream, char openBodyToken /* = '{' */, char closeBodyToken /* = '}' */ )
{
	// Make sure we are not on the top
	if ( readPosition == 0 )
	{
		return false;
	}

	// Step back unless we are just after the bracket
	if ( tokens[readPosition - 1].tokenID != openBodyToken )
	{
		--readPosition;
	}

	// Level counter
	uint32 currentLevel = 1;

	// Extract tokens
	parserToken_t token;
	while ( true )
	{
		// Pop token
		if ( !PopToken( token ) )
		{
			break;
		}

		// Exiting bracket
		if ( token.tokenID == closeBodyToken )
		{
			--currentLevel;
		}

		// Entering bracket
		else if ( token.tokenID == openBodyToken )
		{
			++currentLevel;
		}

		// End of block?
		if ( token.tokenID == closeBodyToken && currentLevel == 0 )
		{
			// We need to unparse the bracket
			--readPosition;
			return true;
		}

		// Push token to output stream
		stream.PushToken( std::forward<parserToken_t>( token ) );
	}

	// Error, not able to extract code from current block
	return false;
}

/*
==================
CParserTokenStream::ExtractInitTokens
==================
*/
bool CParserTokenStream::ExtractInitTokens( CParserTokenStream& stream, char initializeToken /* = '/* =' */, char delimiterToken /* = ';' */ )
{
	// Make sure we are not on the top
	if ( readPosition == 0 )
	{
		return false;
	}

	// Step back unless we are just before the initializeToken
	if ( tokens[readPosition - 1].tokenID != initializeToken )
	{
		--readPosition;
	}

	// Include initializeToken in tokens
	Assert( tokens[readPosition - 1].tokenID == initializeToken );
	--readPosition;

	// Worst case - realloc to no of tokens stored on internal stream
	if ( stream.tokens.empty() )
	{
		stream.tokens.reserve( tokens.size() );
	}

	// Extract tokens
	parserToken_t token;
	int32		  tokenID;
	do
	{
		// Pop token
		if ( !PopToken( token ) )
		{
			// Error, not able to extract code from current block
			return false;
		}

		// Push token to output stream
		tokenID = token.tokenID;
		stream.PushToken( std::forward<parserToken_t>( token ) );

	} while ( tokenID != delimiterToken );

	// We need to unparse the semicolon
	--readPosition;
	return true;
}
