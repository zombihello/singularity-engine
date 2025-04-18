/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef LEXERSTATE_H
#define LEXERSTATE_H

#include "parserlib/lexercontext.h"
#include "parserlib/lexerlistener.h"
#include "parserlib/utils.h"

/**
 * @ingroup parserlib
 * @brief Used to manage the state of the lexer internally
 */
struct parserLexerState_t
{
public:
	/**
	 * @brief Constructor
	 * @param pSource			The raw input buffer
	 * @param pLexerListener	Lexer listener
	 */
	parserLexerState_t( const achar* pSource, CParserLexerListener* pLexerListener )
		: pSource( pSource )
		, scopeLevel( 0 )
		, pLexerListener( pLexerListener )
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
        , pLexerListener( other.pLexerListener )
	{}

	/**
     * @brief Next line
     */
    FORCEINLINE void NextLine()
    {
        ++currentContext.line;
        lineStart = currentContext;
    }

    /**
     * @brief Start to store token
     */
    FORCEINLINE void StoreTokenStart()
    {
        tokenStart = currentContext;
    }

    /**
     * @brief Start to store sequence
     * @param charactersToSkip    Number characters to skip
     */
    FORCEINLINE void StoreSequenceStart( uint32 charactersToSkip = 0 )
    {
        sequenceStart = tokenStart;
        if ( charactersToSkip )
        {
            sequenceStart.byteOffset    += charactersToSkip;
            sequenceStart.charOffset    += charactersToSkip;
        }

        sequenceLineStart = lineStart;
    }

    /**
     * @brief Update current context
     * @param pToken            The token
     * @param tokenByteLength   Length of the token in bytes
     */
    FORCEINLINE void UpdateContext( const achar* pToken, uint32 tokenByteLength )
    {
        currentContext.byteOffset += tokenByteLength;
        currentContext.charOffset += Parserlib_GetNumChars( pToken, tokenByteLength );
    }

    /**
     * @brief Emit token
     * @param tokenID     Token ID
     */
    FORCEINLINE void EmitToken( uint32 tokenID )
    {
        pLexerListener->Token( *this, tokenID );
    }

    /**
     * @brief Emit sequence
     * @param tokenID     Token ID
     */
    FORCEINLINE void EmitSequence( uint32 tokenID )
    {
        pLexerListener->Sequence( *this, tokenID );
    }

    /**
     * @brief Emit comment
     */
    FORCEINLINE void EmitComment()
    {
        pLexerListener->Comment( *this );
    }

    /**
     * @brief Emit error
     * @param pMessage  Message
     */
    FORCEINLINE void EmitError( const achar* pMessage )
    {
        pLexerListener->Error( *this, pMessage );
    }

	const achar*			pSource;			/**< The raw input buffer */
	parserLexerContext_t	currentContext;		/**< The current context of the lexer. Usually this will be synonymous with the "end" of the token emitted */
	parserLexerContext_t	tokenStart;			/**< The start of the token emitted */
	parserLexerContext_t	lineStart;			/**< The context of the current line, at column 0 */
	parserLexerContext_t	sequenceStart;		/**< The start of the sequence emitted (a sequence being a token enclosed in a header/footer, such as a "string" or 'name') */
	parserLexerContext_t	sequenceLineStart;	/**< The context of the line at the start of the sequence, at column 0 */
	uint32					scopeLevel;			/**< The number of opened scopes encountered up till now */

private:
	CParserLexerListener*	pLexerListener;     /**< Lexer listener */
};

#endif // !LEXERSTATE_H