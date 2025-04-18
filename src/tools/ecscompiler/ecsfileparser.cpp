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
{}

/*
==================
CEcsFileParser::ParseFile
==================
*/
bool CEcsFileParser::ParseFile( const achar* pPath, const achar* pCode )
{
	// Setup token output list
	CParserTokenStream		tokens;
	
	// Tokenize the code
	{
		CParserTokenEater		tokenEater( tokens, pPath );
		CParserLexerListener	lexerListener( tokenEater, pPath );
		EcsCode_Tokenize( pCode, &lexerListener );
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

	// Create a component we can only in a module
	if ( pCurrentModule )
	{
		pCurrentComponent = new CEcsStubComponent( *pContext, pName );
		pCurrentModule->AddComponent( pCurrentComponent );
	}
	else
	{
		EmitError( pContext, "A component must be in a module" );
	}
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

	// Add the field into the current component
	if ( pCurrentComponent )
	{
		pCurrentComponent->AddField( new CEcsStubField( *pContext, *pTypeContext, pName, pType ) );
	}
	// Otherwise it is error
	else
	{
		EmitError( pContext, "Fields can be only in components" );
	}
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
	// Otherwise it is error
	else
	{
		EmitError( pContext, "Default field values can be only in components" );
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