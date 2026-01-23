#pragma once

/*
==================
TGrammarInterface::GetNextToken
==================
*/
template<class TFileParserClass, typename TGrammarContextType>
FORCEINLINE uint32 TGrammarInterface<TFileParserClass, TGrammarContextType>::GetNextToken( TGrammarContextType* pContext )
{
	if ( pTokens->IsEndOfStream() )
	{
		return 0;
	}

	// Get next token
	parserToken_t& token = pTokens->GetReadToken();
	pTokens->IncrementReadPosition();

	// Update yystypeFile_t
	pContext->pContext = &token.context;
	pContext->token	   = token.tokenString;

	// Remember current token
	pCurrentToken = &token;
	return token.tokenID;
}

/*
==================
TGrammarInterface::EmitError
==================
*/
template<class TFileParserClass, typename TGrammarContextType>
FORCEINLINE void TGrammarInterface<TFileParserClass, TGrammarContextType>::EmitError( const char* pMessage, TGrammarContextType* pContext )
{
	pFileParser->EmitError( pContext->pContext, S_Sprintf( "%s, near '%s'", pMessage, !pContext->token.empty() ? pContext->token.data() : "<TOKEN_EMPTY>" ).c_str() );
}

/*
==================
TGrammarInterface::GetFileParser
==================
*/
template<class TFileParserClass, typename TGrammarContextType>
FORCEINLINE TFileParserClass* TGrammarInterface<TFileParserClass, TGrammarContextType>::GetFileParser() const
{
	return pFileParser;
}

/*
==================
TGrammarInterface::GetCurrentTokenLine
==================
*/
template<class TFileParserClass, typename TGrammarContextType>
FORCEINLINE int32 TGrammarInterface<TFileParserClass, TGrammarContextType>::GetCurrentTokenLine() const
{
	if ( !pCurrentToken )
	{
		return -1;
	}
	return pCurrentToken->context.line;
}

/*
==================
TGrammarInterface::GetLastTokenLine
==================
*/
template<class TFileParserClass, typename TGrammarContextType>
FORCEINLINE int32 TGrammarInterface<TFileParserClass, TGrammarContextType>::GetLastTokenLine() const
{
	if ( pTokens->IsEmpty() )
	{
		return -1;
	}
	return pTokens->GetLastToken().context.line;
}