#include "parserlib/tokenstream.h"
#include "parserlib/tokeneater.h"
#include "parserlib/lexerlistener.h"
#include "tools/ecscompiler/ecsfileparser.h"

// Entry function to call lexer to tokenize source code
extern void EcsCode_Tokenize( const achar* pSourceCode, CParserLexerListener* pLexerListener );

// Entry function grammar to parse file source code
extern int EcsCode_GrammarFile( CParserTokenStream& tokens, CEcsFileParser& fileParser );


/*
==================
CEcsFileParser::CEcsFileParser
==================
*/
CEcsFileParser::CEcsFileParser( CEcsSystemStub& stubs )
	: bHasError( false )
	, stubs( stubs )
	, pCurrentModule( NULL )
	, pCurrentComponent( NULL )
	, pCurrentSystem( NULL )
{}

/*
==================
CEcsFileParser::ParseFile
==================
*/
bool CEcsFileParser::ParseFile( const achar* pPath, const achar* pCode )
{
	// Setup token output list
	Msg( "EcsCompiler: Parse ECS file '%s'", pPath );
	CParserTokenStream		tokens;
	
	// Tokenize the code
	{
		CParserTokenEater		tokenEater( tokens, pPath );
		CParserLexerListener	lexerListener( tokenEater, pPath );
		EcsCode_Tokenize( pCode, &lexerListener );
		bHasError				= lexerListener.HasError();
	}

	// Parser the code
	int32	result = EcsCode_GrammarFile( tokens, *this );

	// Did we parse stuff correctly?
	return result == 0 && !bHasError;
}

/*
==================
CEcsFileParser::StartModule
==================
*/
void CEcsFileParser::StartModule( const parserFileContext_t* pContext, const achar* pName )
{
	// Make sure that the context and name are valid
	AssertMsg( pContext, "Invalid context for a module" );
	AssertMsg( S_Strlen( pName ) > 0, "Module name isn't valid" );
	
	// We cann't declare a new module in another module
	if ( pCurrentModule )
	{
		EmitError( pContext, "Cannot declare module in module" );
		return;
	}

	pCurrentModule = new CEcsStubModule( *pContext, pName );
	stubs.AddModule( pCurrentModule );
}

/*
==================
CEcsFileParser::AddUsing
==================
*/
void CEcsFileParser::AddUsing( const parserFileContext_t* pContext, const achar* pName )
{
	AssertMsg( pContext, "Invalid context for a using" );
	AssertMsg( S_Strlen( pName ) > 0, "Using name isn't valid" );

	// Using must be in a module
	if ( !pCurrentModule )
	{
		EmitError( pContext, "'using' must be in a module" );
		return;
	}

	pCurrentModule->AddUsing( new CEcsStubUsing( *pContext, pName ) );
}

/*
==================
CEcsFileParser::EndDefinition
==================
*/
void CEcsFileParser::EndDefinition( int32 line, const parserFileContext_t* pScopeStart, const parserFileContext_t* pScopeEnd )
{
	AssertMsg( line > -1, "Invalid end line" );

	// Exit context
	ecsScopeStub_t*		pScope = NULL;
	if ( pCurrentComponent )
	{
		pScope				= &pCurrentComponent->GetScope();
		pCurrentComponent	= NULL;
	}
	else if ( pCurrentResource )
	{
		pScope				= &pCurrentResource->GetScope();
		pCurrentResource	= NULL;
	}
	else if ( pCurrentSystem )
	{
		// A system must have at least one read or write component
		bool	bHasAnyFields = false;
		for ( uint32 accessType = 0; accessType < ECS_FIELD_NUM_ACCESS_TYPES; ++accessType )
		{
			bHasAnyFields |= pCurrentSystem->HasFields( ( ecsFieldAccessType_t )accessType );
		}
		if ( !bHasAnyFields )
		{
			EmitError( pScopeStart, "A system must have at least one read or write component" );
		}

		pScope				= &pCurrentSystem->GetScope();
		pCurrentSystem		= NULL;
	}
	else if ( pCurrentModule )
	{
		pScope			= &pCurrentModule->GetScope();
		pCurrentModule	= NULL;
	}

	AssertMsg( pScope, "No stub to end" );
	pScope->startContext	= *pScopeStart;
	pScope->endContext		= *pScopeEnd;
}

/*
==================
CEcsFileParser::StartComponent
==================
*/
void CEcsFileParser::StartComponent( const parserFileContext_t* pContext, const achar* pName )
{
	AssertMsg( pContext, "Invalid context for a component" );
	AssertMsg( S_Strlen( pName ) > 0, "Component name isn't valid" );

	// Update the metadata scope
	if ( pCurrentMetadata )
	{
		ecsScopeStub_t&		scope = pCurrentMetadata->GetScope();
		scope.startContext	= pCurrentMetadata->GetContext();
		scope.endContext	= *pContext;
	}

	// Create a component we can only in a module
	if ( pCurrentModule )
	{
		pCurrentComponent = new CEcsStubDataType( *pContext, pName, pCurrentMetadata );
		pCurrentModule->AddComponent( pCurrentComponent );
	}
	else
	{
		EmitError( pContext, "A component must be in a module" );
	}
	pCurrentMetadata = NULL;
}

/*
==================
CEcsFileParser::StartResource
==================
*/
void CEcsFileParser::StartResource( const parserFileContext_t* pContext, const achar* pName )
{
	AssertMsg( pContext, "Invalid context for a resource" );
	AssertMsg( S_Strlen( pName ) > 0, "Resource name isn't valid" );

	// Update the metadata scope
	if ( pCurrentMetadata )
	{
		ecsScopeStub_t&		scope = pCurrentMetadata->GetScope();
		scope.startContext	= pCurrentMetadata->GetContext();
		scope.endContext	= *pContext;
	}

	// Create a resource we can only in a module
	if ( pCurrentModule )
	{
		pCurrentResource = new CEcsStubDataType( *pContext, pName, pCurrentMetadata );
		pCurrentModule->AddResource( pCurrentResource );
	}
	else
	{
		EmitError( pContext, "A resource must be in a module" );
	}
	pCurrentMetadata = NULL;
}

/*
==================
CEcsFileParser::AddField
==================
*/
void CEcsFileParser::AddField( const parserFileContext_t* pContext, const parserFileContext_t* pTypeContext, const achar* pName, const achar* pType )
{
	AssertMsg( pContext, "Invalid context for a field" );
	AssertMsg( pTypeContext, "Invalid context for a field type" );
	AssertMsg( S_Strlen( pName ) > 0, "Field name isn't valid" );
	AssertMsg( S_Strlen( pType ) > 0, "Field type isn't valid" );

	// Update the metadata scope
	if ( pCurrentMetadata )
	{
		ecsScopeStub_t&		scope = pCurrentMetadata->GetScope();
		scope.startContext	= pCurrentMetadata->GetContext();
		scope.endContext	= *pContext;
	}

	// Add the field into the current component
	if ( pCurrentComponent )
	{
		pCurrentComponent->AddField( new CEcsStubField( *pContext, *pTypeContext, pName, pType, pCurrentMetadata ) );
	}
	// Add the field into the current resource
	else if ( pCurrentResource )
	{
		pCurrentResource->AddField( new CEcsStubField( *pContext, *pTypeContext, pName, pType, pCurrentMetadata ) );
	}
	// Otherwise it is error
	else
	{
		EmitError( pContext, "Fields can be only in components or resources" );
	}
	pCurrentMetadata = NULL;
}

/*
==================
CEcsFileParser::SetDefaultFieldValue
==================
*/
void CEcsFileParser::SetDefaultFieldValue( const parserFileContext_t* pContext, const parserFileContext_t* pValueContext, const achar* pName, const achar* pValue )
{
	AssertMsg( pContext, "Invalid context for a field" );
	AssertMsg( pValueContext, "Invalid context for a field value" );
	AssertMsg( S_Strlen( pName ) > 0, "Field name isn't valid" );

	// Add the default field value into the current component
	if ( pCurrentComponent )
	{
		pCurrentComponent->AddDefaultFieldValue( new CEcsStubDefaultFieldValue( *pContext, *pValueContext, pName, pValue ) );
	}
	// Add the default field value into the current resource
	else if ( pCurrentResource )
	{
		pCurrentResource->AddDefaultFieldValue( new CEcsStubDefaultFieldValue( *pContext, *pValueContext, pName, pValue ) );
	}
	// Otherwise it is error
	else
	{
		EmitError( pContext, "Default field values can be only in components or resources" );
	}
}

/*
==================
CEcsFileParser::EmitError
==================
*/
void CEcsFileParser::EmitError( const parserFileContext_t* pContext, const achar* pMessage )
{
	Error( "%s: %s", pContext->ToString().c_str(), pMessage );
	bHasError = true;
}

/*
==================
CEcsFileParser::AddMetadata
==================
*/
void CEcsFileParser::AddMetadata( const parserFileContext_t* pContext, const parserFileContext_t* pValueContext, const achar* pName, const achar* pValue )
{
	AssertMsg( pContext, "Invalid context for a metadata" );
	AssertMsg( S_Strlen( pName ) > 0, "Metadata name isn't valid" );
	if ( !pCurrentMetadata )
	{
		pCurrentMetadata = new CEcsStubMetadata( *pContext );
	}
	pCurrentMetadata->AddValue( new CEcsStubMetadataValue( *pContext, pValueContext, pName, pValue ? pValue : "" ) );
}

/*
==================
CEcsFileParser::StartSystem
==================
*/
void CEcsFileParser::StartSystem( const parserFileContext_t* pContext, const achar* pName )
{
	AssertMsg( pContext, "Invalid context for a system" );
	AssertMsg( S_Strlen( pName ) > 0, "System name isn't valid" );

	// Update the metadata scope
	if ( pCurrentMetadata )
	{
		ecsScopeStub_t&		scope = pCurrentMetadata->GetScope();
		scope.startContext	= pCurrentMetadata->GetContext();
		scope.endContext	= *pContext;
	}

	// Create a system we can only in a module
	if ( pCurrentModule )
	{
		pCurrentSystem = new CEcsStubSystem( *pContext, pName, pCurrentMetadata );
		pCurrentModule->AddSystem( pCurrentSystem );
	}
	else
	{
		EmitError( pContext, "A system must be in a module" );
	}
	pCurrentMetadata = NULL;
}

/*
==================
CEcsFileParser::SetSystemStage
==================
*/
void CEcsFileParser::SetSystemStage( const parserFileContext_t* pContext, ecsSystemStage_t stage )
{
	AssertMsg( pContext, "Invalid context for a system field" );

	// Set the stage into the current system
	if ( pCurrentSystem )
	{
		pCurrentSystem->SetStage( *pContext, stage );
	}
	// Otherwise it is error
	else
	{
		EmitError( pContext, "System stage can be only in systems" );
	}
}

/*
==================
CEcsFileParser::AddSystemField
==================
*/
void CEcsFileParser::AddSystemField( const parserFileContext_t* pContext, const parserFileContext_t* pTypeContext, const achar* pName, const achar* pType, ecsFieldAccessType_t accessType )
{
	AssertMsg( pContext, "Invalid context for a system field" );
	AssertMsg( pTypeContext, "Invalid context for a system field type" );
	AssertMsg( S_Strlen( pName ) > 0, "System field name isn't valid" );
	AssertMsg( S_Strlen( pType ) > 0, "System field type isn't valid" );

	// Add the field into the current system
	if ( pCurrentSystem )
	{
		pCurrentSystem->AddField( accessType, new CEcsStubField( *pContext, *pTypeContext, pName, pType ) );
	}
	// Otherwise it is error
	else
	{
		EmitError( pContext, "System fields can be only in systems" );
	}
}

/*
==================
CEcsFileParser::AddSystemFilter
==================
*/
void CEcsFileParser::AddSystemFilter( const parserFileContext_t* pContext, const achar* pName, ecsSystemFilterType_t filterType )
{
	AssertMsg( pContext, "Invalid context for a system filter" );
	AssertMsg( S_Strlen( pName ) > 0, "System filter isn't valid" );

	// Add the filter into the current system
	if ( pCurrentSystem )
	{
		pCurrentSystem->AddFilter( filterType, new CEcsStubSystemFilter( *pContext, pName ) );
	}
	// Otherwise it is error
	else
	{
		EmitError( pContext, "System filters can be only in systems" );
	}
}