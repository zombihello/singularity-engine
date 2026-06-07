#pragma once

/*
==================
CCmdArgs::CCmdArgs
==================
*/
FORCEINLINE CCmdArgs::CCmdArgs()
{
}

/*
==================
CCmdArgs::CCmdArgs
==================
*/
FORCEINLINE CCmdArgs::CCmdArgs( const char* pArgs )
{
	TokenizeString( pArgs );
}

/*
==================
CCmdArgs::CCmdArgs
==================
*/
FORCEINLINE CCmdArgs::CCmdArgs( const CCmdArgs& other )
	: tokenized( other.tokenized )
{
	BuildArgv();
}

/*
==================
CCmdArgs::Argc
==================
*/
FORCEINLINE uint32 CCmdArgs::Argc() const
{
	return (uint32)argv.size();
}

/*
==================
CCmdArgs::Argv
==================
*/
FORCEINLINE const char* CCmdArgs::Argv( uint32 arg ) const
{
	return arg < (uint32)argv.size() ? argv[arg] : "";
}

/*
==================
CCmdArgs::Clear
==================
*/
FORCEINLINE void CCmdArgs::Clear()
{
	argv.clear();
	tokenized.clear();
}

/*
==================
CCmdArgs::AppendArg
==================
*/
FORCEINLINE void CCmdArgs::AppendArg( const char* pArg )
{
	// Do nothing if the arg isn't valid
	if ( !pArg )
	{
		return;
	}

	// Append the argument
	uint32 argLen	 = S_Strlen( pArg );
	uint32 argOffset = (uint32)tokenized.size();
	tokenized.resize( argOffset + argLen + 1 );
	Mem_Memcpy( tokenized.data() + argOffset, pArg, argLen );
	tokenized.back() = '\0';

	// Rebuild the argv array
	BuildArgv();
}

/*
==================
CCmdArgs::Args
==================
*/
FORCEINLINE eastl::string CCmdArgs::Args( uint32 start /* = INVALID_INDEX */, uint32 end /* = INVALID_INDEX */, bool bEscapeArgs /* = false */ ) const
{
	eastl::string result;
	Args( result, start, end, bEscapeArgs );
	return result;
}

/*
==================
CCmdArgs::Args
==================
*/
FORCEINLINE const char* const* CCmdArgs::Args( uint32& argc ) const
{
	argc = (uint32)argv.size();
	return argv.data();
}

/*
==================
CCmdArgs::operator=
==================
*/
FORCEINLINE void CCmdArgs::operator=( const CCmdArgs& args )
{
	// Copy the tokenized array and rebuild the argv array
	tokenized = args.tokenized;
	BuildArgv();
}
