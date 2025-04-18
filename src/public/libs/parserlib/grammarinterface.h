/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef GRAMMARINTERFACE_H
#define GRAMMARINTERFACE_H

#include "parserlib/tokenstream.h"

/**
 * @ingroup parserlib
 * @brief Grammar interface
 */
template<class TFileParserClass, typename TGrammarContextType>
class TGrammarInterface
{
public:
    /**
     * @brief Constructor
     * @param tokens       Tokens stream
     * @param fileParser   File parser
     */
    TGrammarInterface( CParserTokenStream& tokens, TFileParserClass& fileParser )
        : pTokens( &tokens )
        , pFileParser( &fileParser )
        , pCurrentToken( NULL )
    {}

    /**
     * @brief Get next token
     * @param pContext   Grammar context
     * @return Return ID of the next token. If is end of stream returns 0
     */
    FORCEINLINE uint32 GetNextToken( TGrammarContextType* pContext )
    {
        if ( pTokens->IsEndOfStream() )
        {
            return 0;
        }

        // Get next token
        parserToken_t&      token = pTokens->GetReadToken();
        pTokens->IncrementReadPosition();

        // Update yystypeFile_t
        pContext->pContext  = &token.context;
        pContext->token     = token.tokenString;

        // Remember current token
        pCurrentToken       = &token;
        return token.tokenID;
    }

    /**
     * @brief Emit error
     * @param pMessage    Error message
     * @param pContext    Grammar context
     */
    FORCEINLINE void EmitError( const achar* pMessage, TGrammarContextType* pContext )
    {
        pFileParser->EmitError( pContext->pContext, S_Sprintf( "%s, near '%s'", pMessage, !pContext->token.empty() ? pContext->token.data() : "<TOKEN_EMPTY>" ).c_str() );
    }

    /**
     * @brief Get file parser
     * @return Return pointer to the file parser
     */
    FORCEINLINE TFileParserClass* GetFileParser() const
    {
        return pFileParser;
    }

    /**
     * @brief Get current token line
     * @return Return current token line. If a token isn't valid returns -1
     */
    FORCEINLINE int32 GetCurrentTokenLine() const
    {
        if ( !pCurrentToken )
        {
            return -1;
        }
        return pCurrentToken->context.line;
    }

    /**
     * @brief Get last token line
     * @return Returns the line of the last token in current context. If a token isn't valid returns -1
     */
    FORCEINLINE int32 GetLastTokenLine() const
    {
        if ( pTokens->IsEmpty() )
        {
            return -1;
        }
        return pTokens->GetLastToken().context.line;
    }

private:
    CParserTokenStream*     pTokens;         /**< Tokens stream */
    TFileParserClass*       pFileParser;     /**< File parser */
    parserToken_t*          pCurrentToken;   /**< Current token */
};

#endif // !GRAMMARINTERFACE_H