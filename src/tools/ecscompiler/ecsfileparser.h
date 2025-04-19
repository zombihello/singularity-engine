#ifndef ECSFILEPARSER_H
#define ECSFILEPARSER_H

#include "core/types.h"
#include "parserlib/filecontext.h"
#include "tools/ecscompiler/ecsfieldstubs.h"

//-----------------------------------------------------------------------------
// ECS file parser
//-----------------------------------------------------------------------------
class CEcsFileParser
{
public:
	CEcsFileParser( CEcsSystemStub& stubs );
	bool ParseFile( const achar* pPath, const achar* pCode );
	
	void StartModule( const parserFileContext_t* pContext, const achar* pName );
	void AddUsing( const parserFileContext_t* pContext, const achar* pName );
	void EndDefinition( int32 line, const parserFileContext_t* pScopeStart, const parserFileContext_t* pScopeEnd );
	void StartComponent( const parserFileContext_t* pContext, const achar* pName );
	void AddField( const parserFileContext_t* pContext, const parserFileContext_t* pTypeContext, const achar* pName, const achar* pType );
	void SetDefaultFieldValue( const parserFileContext_t* pContext, const parserFileContext_t* pValueContext, const achar* pName, const achar* pValue );
	void EmitError( const parserFileContext_t* pContext, const achar* pMessage );

private:
	bool				bHasError;
	CEcsSystemStub&		stubs;
	CEcsStubModule*		pCurrentModule;
	CEcsStubComponent*	pCurrentComponent;
};

#endif // !ECSFILEPARSER_H