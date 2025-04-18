/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef TOKENEATER_H
#define TOKENEATER_H

#include "parserlib/tokenstream.h"
#include "parserlib/stringbuffer.h"

/**
 * @ingroup parserlib
 * @brief Connection interface between lexer and a file parser
 */
class CParserTokenEater
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param tokens		Stream tokens. Here will be places all of parsed tokens
	 * @param pPath			File path for context
	 */
	CParserTokenEater( CParserTokenStream& tokens, const achar* pPath );

	/**
	 * @brief Called when token has been emitted
	 * @param line				Line where the start of the token emitted
	 * @param tokenID			Token ID
	 * @param pTokenString		Token in string format
	 * @param tokenStringSize	Token string size
	 * @param scopeLevel		The number of opened scopes encountered up till now
	 * @param charLineStart		Character position at the start of the line
	 * @param charStart			Character position this context begins at
	 * @param charEnd			Character position at the end of the context
	 */
	virtual void OnEmitToken( uint32 line, uint32 tokenID, const achar* pTokenString, uint32 tokenStringSize, uint32 scopeLevel, uint32 charLineStart, uint32 charStart, uint32 charEnd );

	/**
	 * @brief Called when comment has been emitted
	 * @param line			Line where the start of the comment emitted
	 * @param pText			Comment text
	 * @param textSize		Comment text size
	 * @param scopeLevel	The number of opened scopes encountered up till now
	 * @param charLineStart	Character position at the start of the line
	 * @param charStart		Character position this context begins at
	 * @param charEnd		Character position at the end of the context
	 */
	virtual void OnEmitComment( uint32 line, const achar* pText, uint32 textSize, uint32 scopeLevel, uint32 charLineStart, uint32 charStart, uint32 charEnd );
	
	/**
	 * @brief Called when has been error
	 * @param line			Line where the start of the error emitted
	 * @param pMessage		Error message
	 */
	virtual void OnEmitError( uint32 line, const achar* pMessage );

protected:
	std::string					path;		/**< File path for context */
	CParserTokenStream&			tokens;		/**< Stream tokens. Here will be places all of parsed tokens */
};

#endif // !TOKENEATER_H