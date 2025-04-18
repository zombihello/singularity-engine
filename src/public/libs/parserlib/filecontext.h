/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef FILECONTEXT_H
#define FILECONTEXT_H

#include "parserlib/stringbuffer.h"

/**
 * @ingroup parserlib
 * @brief File context info for parser data
 */
struct parserFileContext_t
{
	/**
	 * @brief Constructor
	 */
	parserFileContext_t()
		: line( 0 )
		, charLineStart( 0 )
		, charPosition( 0 )
		, charEndPosition( 0 )
	{}

	/**
	 * @brief Constructor
	 * @param file				Referenced file
	 * @param line				Line this context starts on
	 * @param charLineStart		Character position at the start of the line
	 * @param charPosition		Character position this context begins at
	 * @param charEndPosition	Character position at the end of the context
	 */
	parserFileContext_t( const parserStringBufferANSI_t& file, int32 line, int32 charLineStart, int32 charPosition, int32 charEndPosition )
		: file( file )
		, line( line )
		, charLineStart( charLineStart )
		, charPosition( charPosition )
		, charEndPosition( charEndPosition )
	{}

	/**
	 * @brief Constructor
	 * @param file		Referenced file
	 * @param line		Line this context starts on
	 */
	parserFileContext_t( const parserStringBufferANSI_t& file, int32 line )
		: parserFileContext_t( file, line, 0, 0, 0 )
	{}

	/**
	 * @brief Constructor
	 * @param other		Other a parser file context
	 */
	parserFileContext_t( const parserFileContext_t& other )
		: file( other.file )
		, line( other.line )
		, charLineStart( other.charLineStart )
		, charPosition( other.charPosition )
		, charEndPosition( other.charEndPosition )
	{}

	/**
	 * @brief Constructor
	 * @param other		Other a parser file context
	 */
	parserFileContext_t( parserFileContext_t&& other )
		: file( std::move( other.file ) )
		, line( std::move( other.line ) )
		, charLineStart( std::move( other.charLineStart ) )
		, charPosition( std::move( other.charPosition ) )
		, charEndPosition( std::move( other.charEndPosition ) )
	{}

	/**
	 * @brief Copy operator
	 * @param other		Other a parser file context
	 * @return Return reference to self
	 */
	FORCEINLINE parserFileContext_t& operator=( const parserFileContext_t& other )
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

	/**
	 * @brief Copy operator (move version)
	 * @param other		Other a parser file context
	 * @return Return reference to self
	 */
	FORCEINLINE parserFileContext_t& operator=( parserFileContext_t&& other )
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

	/**
	 * @brief Convert the context to string
	 * @return Return converted the context to string
	 */
	FORCEINLINE std::string ToString() const
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

	parserStringBufferANSI_t		file;				/**< Referenced file */
	int32							line;				/**< Line this context starts on */
	int32							charLineStart;		/**< Character position at the start of the line */
	int32							charPosition;		/**< Character position this context begins at */
	int32							charEndPosition;	/**< Character position at the end of the context */
};

#endif // !FILECONTEXT_H