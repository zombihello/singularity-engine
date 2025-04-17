/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef LEXER_EMITTER_H
#define LEXER_EMITTER_H

#include "parserlib/lexer_listener.h"
#include "parserlib/token_eater.h"

/**
 * @ingroup parserlib
 * @brief Lexer tokens emitter
 */
class CParserLexerEmitter : public CParserLexerListenerBase
{
public:
	/**
	 * @brief Constructor
	 * @param parserListener	Parser listener
	 * @param pPath				File path
	 */
	CParserLexerEmitter( CParserTokenEater& parserListener, const achar* pPath );

	/**
	 * @brief A Script comment has been encountered (could be either single or multiline)
	 * @param lexerState		Lexer state
	 */
	virtual void Comment( const parserLexerState_t& lexerState ) override;

	/**
	 * @brief A Standard token
	 * @param lexerState		Lexer state
	 * @param tokenID			Token ID
	 */
	virtual void Token( const parserLexerState_t& lexerState, uint32 tokenID ) override;

	/**
	 * @brief A Token denoted by a start and end series of chars (such as a string or name)
	 * @param lexerState		Lexer state
	 * @param tokenID			Token ID
	 */
	virtual void Sequence( const parserLexerState_t& lexerState, uint32 tokenID ) override;

	/**
	 * @brief A section of script that has failed to match any token rules
	 * @param lexerState		Lexer state
	 * @param pMessage			Error message
	 */
	virtual void Error( const parserLexerState_t& lexerState, const achar* pMessage ) override;

private:
	CParserTokenEater&		parserListener;	/**< Parser listener */
	std::string				path;			/**< File path */
};

#endif // !LEXER_EMITTER_H