#ifndef FILECONTEXT_H
#define FILECONTEXT_H

#include "parserlib/stringbuffer.h"

//-----------------------------------------------------------------------------
// File context info for parser data
//-----------------------------------------------------------------------------
struct parserFileContext_t
{
	parserFileContext_t()
		: line( 0 )
		, charLineStart( 0 )
		, charPosition( 0 )
		, charEndPosition( 0 )
	{}

	parserFileContext_t( const parserStringBufferANSI_t& file, int32 line, int32 charLineStart, int32 charPosition, int32 charEndPosition )
		: file( file )
		, line( line )
		, charLineStart( charLineStart )
		, charPosition( charPosition )
		, charEndPosition( charEndPosition )
	{}

	parserFileContext_t( const parserStringBufferANSI_t& file, int32 line )
		: parserFileContext_t( file, line, 0, 0, 0 )
	{}

	parserFileContext_t( const parserFileContext_t& other )
		: file( other.file )
		, line( other.line )
		, charLineStart( other.charLineStart )
		, charPosition( other.charPosition )
		, charEndPosition( other.charEndPosition )
	{}

	parserFileContext_t( parserFileContext_t&& other )
		: file( std::move( other.file ) )
		, line( std::move( other.line ) )
		, charLineStart( std::move( other.charLineStart ) )
		, charPosition( std::move( other.charPosition ) )
		, charEndPosition( std::move( other.charEndPosition ) )
	{}

	parserFileContext_t& operator=( const parserFileContext_t& other );
	parserFileContext_t& operator=( parserFileContext_t&& other );
	std::string ToString() const;

	parserStringBufferANSI_t		file;				// Referenced file
	int32							line;				// Line this context starts on
	int32							charLineStart;		// Character position at the start of the line
	int32							charPosition;		// Character position this context begins at
	int32							charEndPosition;	// Character position at the end of the context
};

#include "parserlib/filecontext.inl"

#endif // !FILECONTEXT_H