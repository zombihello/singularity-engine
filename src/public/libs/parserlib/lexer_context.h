/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef LEXER_CONTEXT_H
#define LEXER_CONTEXT_H

#include "core/platform.h"
#include "core/types.h"

/**
 * @ingroup parserlib
 * @brief Parser lexer context
 */
struct parserLexerContext_t
{
	/**
	 * @brief Constructor
	 */
	parserLexerContext_t()
		: line( 0 )
		, byteOffset( 0 )
		, charOffset( 0 )
	{}

	/**
	 * @brief Constructor
	 * @param other		Other a lexer context
	 */
	parserLexerContext_t( const parserLexerContext_t& other )
		: line( other.line )
		, byteOffset( other.byteOffset )
		, charOffset( other.charOffset )
	{}

	uint32		line;		/**< The Line that this context refers to */
	uint32		byteOffset;	/**< The associated position in the raw input buffer */
	uint32		charOffset; /**< The associated position in the input buffer resolved as a utf-8 string */
};

#endif // !LEXER_CONTEXT_H