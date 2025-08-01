#pragma once

/*
==================
parserFileContext_t::operator=
==================
*/
FORCEINLINE parserFileContext_t& parserFileContext_t::operator=( const parserFileContext_t& other )
{
	if ( this != &other )
	{
		file			= other.file;
		line			= other.line;
		charLineStart	= other.charLineStart;
		charPosition	= other.charPosition;
		charEndPosition = other.charEndPosition;
	}
	return *this;
}

/*
==================
parserFileContext_t::operator=
==================
*/
FORCEINLINE parserFileContext_t& parserFileContext_t::operator=( parserFileContext_t&& other )
{
	if ( this != &other )
	{
		file			= std::move( other.file );
		line			= std::move( other.line );
		charLineStart	= std::move( other.charLineStart );
		charPosition	= std::move( other.charPosition );
		charEndPosition	= std::move( other.charEndPosition );
	}
	return *this;
}

/*
==================
parserFileContext_t::ToString
==================
*/
FORCEINLINE std::string parserFileContext_t::ToString() const
{
	if ( line != -1 )
	{
		return S_Sprintf( "%s[%i]", file.AsChar(), line );
	}
	else if ( !file.IsEmpty() )
	{
		return S_Sprintf( "%s", file.AsChar() );
	}
	else
	{
		return "Unknown";
	}
}
