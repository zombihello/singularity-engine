#pragma once
#include "tier0/types.h"
#include "utils/parserlib/filecontext.h"
#include "tools/ecscompiler/ecsfieldstubs.h"

//-----------------------------------------------------------------------------
// ECS file parser
//-----------------------------------------------------------------------------
class CEcsFileParser
{
public:
	CEcsFileParser( CEcsSystemStub& stubs );
	bool ParseFile( const char* pPath, const char* pCode );

	// Generate parser functions
	void EndDefinition( int32 line, const parserFileContext_t* pScopeStart, const parserFileContext_t* pScopeEnd );
	void EmitError( const parserFileContext_t* pContext, const char* pMessage );
	void AddMetadata( const parserFileContext_t* pContext, ecsMetadataType_t type );
	void AddMetadata( const parserFileContext_t* pContext, const parserFileContext_t* pValueContext, ecsMetadataType_t type, const char* pValue );

	// Module functions
	void StartModule( const parserFileContext_t* pContext, const char* pName );
	void AddUsing( const parserFileContext_t* pContext, const char* pName );

	// Component and resource functions
	void StartComponent( const parserFileContext_t* pContext, const char* pName );
	void StartResource( const parserFileContext_t* pContext, const char* pName );
	void AddField( const parserFileContext_t* pContext, const parserFileContext_t* pTypeContext, const char* pName, const char* pType );
	void SetDefaultFieldValue( const parserFileContext_t* pContext, const parserFileContext_t* pValueContext, const char* pName, const char* pValue );

	// System functions
	void StartSystem( const parserFileContext_t* pContext, const char* pName );
	void AddSystemField( const parserFileContext_t* pContext, const parserFileContext_t* pTypeContext, const char* pName, const char* pType, ecsFieldAccessType_t accessType );
	void AddSystemFilter( const parserFileContext_t* pContext, const char* pName, ecsSystemFilterType_t filterType );

private:
	bool					  bHasError;
	CEcsSystemStub&			  stubs;
	CEcsStubModule*			  pCurrentModule;
	CEcsStubDataType*		  pCurrentComponent;
	CEcsStubDataType*		  pCurrentResource;
	CEcsStubSystem*			  pCurrentSystem;
	TRefPtr<CEcsStubMetadata> pCurrentMetadata;
};
