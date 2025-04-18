/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef LEXERLISTENER_H
#define LEXERLISTENER_H

#include "parserlib/tokeneater.h"

// Forward declarations
struct parserLexerState_t;

/**
 * @ingroup parserlib
 * @brief Parser lexer listener interface
 * 
 * You need to create a subclass of CParserLexerListener in
 * order to receive the tokenised output of a lexer
 */
class CParserLexerListener
{
public:
	/**
	 * @brief Constructor
	 * @param parserListener	Parser listener
	 * @param pPath				File path
	 */
	CParserLexerListener( CParserTokenEater& parserListener, const achar* pPath );

	/**
	 * @brief A Script comment has been encountered (could be either single or multiline)
	 * @param lexerState		Lexer state
	 */
	virtual void Comment( const parserLexerState_t& lexerState );

	/**
	 * @brief A Standard token
	 * @param lexerState		Lexer state
	 * @param tokenID			Token ID
	 */
	virtual void Token( const parserLexerState_t& lexerState, uint32 tokenID );

	/**
	 * @brief A Token denoted by a start and end series of chars (such as a string or name)
	 * @param lexerState		Lexer state
	 * @param tokenID			Token ID
	 */
	virtual void Sequence( const parserLexerState_t& lexerState, uint32 tokenID );

	/**
	 * @brief A section of script that has failed to match any token rules
	 * @param lexerState		Lexer state
	 * @param pMessage			Error message
	 */
	virtual void Error( const parserLexerState_t& lexerState, const achar* pMessage );

protected:
	CParserTokenEater&		parserListener;		/**< Parser listener */
	std::string				path;				/**< File path */
};

#endif // !LEXERLISTENER_H