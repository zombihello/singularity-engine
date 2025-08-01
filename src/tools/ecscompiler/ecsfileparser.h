#pragma once
#include "stdlib/types.h"
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
	void AddMetadata( const parserFileContext_t* pContext, ecsMetadataType_t type );
	void AddMetadata( const parserFileContext_t* pContext, const parserFileContext_t* pValueContext, ecsMetadataType_t type, const achar* pValue );

	// Module functions
	void StartModule( const parserFileContext_t* pContext, const achar* pName );
	void AddUsing( const parserFileContext_t* pContext, const achar* pName );

	// Component and resource functions
	void StartComponent( const parserFileContext_t* pContext, const achar* pName );
	void StartResource( const parserFileContext_t* pContext, const achar* pName );
	void AddField( const parserFileContext_t* pContext, const parserFileContext_t* pTypeContext, const achar* pName, const achar* pType );
	void SetDefaultFieldValue( const parserFileContext_t* pContext, const parserFileContext_t* pValueContext, const achar* pName, const achar* pValue );
	
	// System functions
	void StartSystem( const parserFileContext_t* pContext, const achar* pName );
	void AddSystemField( const parserFileContext_t* pContext, const parserFileContext_t* pTypeContext, const achar* pName, const achar* pType, ecsFieldAccessType_t accessType );
	void AddSystemFilter( const parserFileContext_t* pContext, const achar* pName, ecsSystemFilterType_t filterType );

private:
	bool							bHasError;
	CEcsSystemStub&					stubs;
	CEcsStubModule*					pCurrentModule;
	CEcsStubDataType*				pCurrentComponent;
	CEcsStubDataType*				pCurrentResource;
	CEcsStubSystem*					pCurrentSystem;
	TRefPtr<CEcsStubMetadata>		pCurrentMetadata;
};