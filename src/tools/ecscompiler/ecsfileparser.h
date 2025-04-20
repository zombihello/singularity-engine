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
	
	// Generate parser functions
	void EndDefinition( int32 line, const parserFileContext_t* pScopeStart, const parserFileContext_t* pScopeEnd );
	void EmitError( const parserFileContext_t* pContext, const achar* pMessage );
	void AddMetadata( const parserFileContext_t* pContext, const parserFileContext_t* pValueContext, const achar* pName, const achar* pValue );

	// Module functions
	void StartModule( const parserFileContext_t* pContext, const achar* pName );
	void AddUsing( const parserFileContext_t* pContext, const achar* pName );

	// Component functions
	void StartComponent( const parserFileContext_t* pContext, const achar* pName );
	void AddComponentField( const parserFileContext_t* pContext, const parserFileContext_t* pTypeContext, const achar* pName, const achar* pType );
	void SetComponentDefaultFieldValue( const parserFileContext_t* pContext, const parserFileContext_t* pValueContext, const achar* pName, const achar* pValue );
	
	// System functions
	void StartSystem( const parserFileContext_t* pContext, const achar* pName );
	void SetSystemStage( const parserFileContext_t* pContext, ecsSystemStage_t stage );
	void AddSystemField( const parserFileContext_t* pContext, const parserFileContext_t* pTypeContext, const achar* pName, const achar* pType, ecsFieldAccessType_t accessType );
	void AddSystemFilter( const parserFileContext_t* pContext, const achar* pName, ecsSystemFilterType_t filterType );

private:
	bool							bHasError;
	CEcsSystemStub&					stubs;
	CEcsStubModule*					pCurrentModule;
	CEcsStubComponent*				pCurrentComponent;
	CEcsStubSystem*					pCurrentSystem;
	TRefPtr<CEcsStubMetadata>		pCurrentMetadata;
};

#endif // !ECSFILEPARSER_H