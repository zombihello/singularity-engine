#pragma once
#include "utils/parserlib/tokenstream.h"

//-----------------------------------------------------------------------------
// Grammar interface
//-----------------------------------------------------------------------------
template<class TFileParserClass, typename TGrammarContextType>
class CGrammarInterface
{
public:
	CGrammarInterface( CParserTokenStream& tokens, TFileParserClass& fileParser )
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

#include "utils/parserlib/grammarinterface.inl"
