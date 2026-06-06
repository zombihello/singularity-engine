#include "pch_tier1.h"
#include "tier1/cmdargs.h"

/*
==================
CCmdArgs::CCmdArgs
==================
*/
CCmdArgs::CCmdArgs( uint32 argc, const char** argv )
{
	// Append arguments into the tokenized array
	for ( uint32 index = 0; index < argc; ++index )
	{
		uint32 argLen	 = S_Strlen( argv[index] );
		uint32 argOffset = (uint32)tokenized.size();
		tokenized.resize( argOffset + argLen + 1 );
		Mem_Memcpy( tokenized.data() + argOffset, argv[index], argLen );
		tokenized.back() = '\0';
	}

	// Rebuild the argv array
	BuildArgv();
}

/*
==================
CCmdArgs::TokenizeString

Parses the given string into command line tokens.
The text is copied to a separate buffer and 0 characters
are inserted in the appropriate place. The argv array
will point into this temporary buffer
==================
*/
void CCmdArgs::TokenizeString( const char* pArgs )
{
	// Do nothing if the pArgs isn't valid
	Clear();
	if ( !pArgs )
	{
		return;
	}

	// Reserve some spaces in the tokenized array
	tokenized.reserve( S_Strlen( pArgs ) );

	// Parses the given string into command line tokens
	while ( *pArgs )
	{
		// Skip spaces
		while ( *pArgs && S_IsSpace( *pArgs ) )
		{
			++pArgs;
		}

		// Make sure that we are not at the end of the given string
		if ( !*pArgs )
		{
			break;
		}

		// Get quoted string
		if ( *pArgs == '"' )
		{
			++pArgs;
			while ( *pArgs && *pArgs != '"' )
			{
				tokenized.emplace_back( *pArgs );
				++pArgs;
			}

			tokenized.emplace_back( '\0' );
			if ( *pArgs == '"' )
			{
				++pArgs;
			}
		}
		// Get unquoted string
		else
		{
			while ( *pArgs && !S_IsSpace( *pArgs ) )
			{
				tokenized.emplace_back( *pArgs );
				++pArgs;
			}
			tokenized.emplace_back( '\0' );
		}
	}

	// Save tokens start into the argv array
	BuildArgv();
}

/*
==================
CCmdArgs::Args
==================
*/
void CCmdArgs::Args( eastl::string& dest, uint32 start /* = INVALID_INDEX */, uint32 end /* = INVALID_INDEX */, bool bEscapeArgs /* = false */ ) const
{
	// Do nothing if we haven't any arg
	if ( argv.empty() )
	{
		return;
	}

	// Validate indices
	uint32 numArgv = (uint32)argv.size();
	if ( start == INVALID_INDEX )
	{
		start = 0;
	}
	if ( end == INVALID_INDEX )
	{
		end = numArgv - 1;
	}
	if ( end >= numArgv )
	{
		end = numArgv - 1;
	}
	Assert( start <= end );
	Assert( start < numArgv );

	// Build a merged string
	if ( bEscapeArgs )
	{
		dest += "\"";
	}
	for ( uint32 index = start; index <= end; ++index )
	{
		if ( index > start )
		{
			if ( bEscapeArgs )
			{
				dest += "\" \"";
			}
			else
			{
				dest += " ";
			}
		}

		dest += bEscapeArgs ? S_EscapeString( argv[index] ) : argv[index];
	}
	if ( bEscapeArgs )
	{
		dest += "\"";
	}
}

/*
==================
CCmdArgs::BuildArgv
==================
*/
void CCmdArgs::BuildArgv()
{
	argv.clear();
	bool bNewToken = true;
	for ( uint32 index = 0, count = (uint32)tokenized.size(); index < count; ++index )
	{
		if ( bNewToken )
		{
			argv.emplace_back( &tokenized[index] );
			bNewToken = false;
		}
		bNewToken = tokenized[index] == '\0';
	}
}
