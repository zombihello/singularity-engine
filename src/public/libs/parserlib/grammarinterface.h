#pragma once
#include "parserlib/tokenstream.h"

//-----------------------------------------------------------------------------
// Grammar interface
//-----------------------------------------------------------------------------
template<class TFileParserClass, typename TGrammarContextType>
class TGrammarInterface
{
public:
	TGrammarInterface( CParserTokenStream& tokens, TFileParserClass& fileParser )
		: pTokens( &tokens )
		, pFileParser( &fileParser )
		, pCurrentToken( NULL )
	{
	}

	uint32 GetNextToken( TGrammarContextType* pContext );
	void   EmitError( const char* pMessage, TGrammarContextType* pContext );

	TFileParserClass* GetFileParser() const;
	int32			  GetCurrentTokenLine() const;
	int32			  GetLastTokenLine() const;

private:
	CParserTokenStream* pTokens;
	TFileParserClass*	pFileParser;
	parserToken_t*		pCurrentToken;
};

#include "parserlib/grammarinterface.inl"