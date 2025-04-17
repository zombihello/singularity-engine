/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef LEXER_LISTENER_H
#define LEXER_LISTENER_H

#include "parserlib/lexer_state.h"

/**
 * @ingroup parserlib
 * @brief Parser lexer listener interface
 * 
 * You need to create a subclass of CParserLexerListenerBase in
 * order to receive the tokenised output of a lexer
 */
class CParserLexerListenerBase
{
public:
	/**
	 * @brief A Script comment has been encountered (could be either single or multiline)
	 * @param lexerState		Lexer state
	 */
	virtual void Comment( const parserLexerState_t& lexerState ) {}

	/**
	 * @brief A Standard token
	 * @param lexerState		Lexer state
	 * @param tokenID			Token ID
	 */
	virtual void Token( const parserLexerState_t& lexerState, uint32 tokenID ) {}

	/**
	 * @brief A Token denoted by a start and end series of chars (such as a string or name)
	 * @param lexerState		Lexer state
	 * @param tokenID			Token ID
	 */
	virtual void Sequence( const parserLexerState_t& lexerState, uint32 tokenID ) {}
	
	/**
	 * @brief A section of script that has failed to match any token rules
	 * @param lexerState		Lexer state
	 * @param pMessage			Error message
	 */
	virtual void Error( const parserLexerState_t& lexerState, const achar* pMessage ) {}
};

#endif // !LEXER_LISTENER_H