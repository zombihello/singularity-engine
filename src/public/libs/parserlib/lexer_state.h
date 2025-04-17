/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef LEXER_STATE_H
#define LEXER_STATE_H

#include "parserlib/lexer_context.h"

/**
 * @ingroup parserlib
 * @brief Used to manage the state of the lexer internally
 */
struct parserLexerState_t
{
public:
	/**
	 * @brief Constructor
	 * @param pSource	The raw input buffer
	 */
	parserLexerState_t( const achar* pSource )
		: pSource( pSource )
		, scopeLevel( 0 )
	{}

	/**
	 * @brief Constructor
	 * @param other		Other a lexer state
	 */
	parserLexerState_t( const parserLexerState_t& other )
		: pSource( other.pSource )
		, currentContext( other.currentContext )
		, tokenStart( other.tokenStart )
		, lineStart( other.lineStart )
		, sequenceStart( other.sequenceStart )
		, sequenceLineStart( other.sequenceLineStart )
		, scopeLevel( other.scopeLevel )
	{}

	const achar*			pSource;			/**< The raw input buffer */
	parserLexerContext_t	currentContext;		/**< The current context of the lexer. Usually this will be synonymous with the "end" of the token emitted */
	parserLexerContext_t	tokenStart;			/**< The start of the token emitted */
	parserLexerContext_t	lineStart;			/**< The context of the current line, at column 0 */
	parserLexerContext_t	sequenceStart;		/**< The start of the sequence emitted (a sequence being a token enclosed in a header/footer, such as a "string" or 'name') */
	parserLexerContext_t	sequenceLineStart;	/**< The context of the line at the start of the sequence, at column 0 */
	uint32					scopeLevel;			/**< The number of opened scopes encountered up till now */
};

#endif // !LEXER_STATE_H