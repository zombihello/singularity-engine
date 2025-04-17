#include "tools/ecscompiler/ecsfieldstubs.h"

/*
==================
CEcsStubBase::CEcsStubBase
==================
*/
CEcsStubBase::CEcsStubBase( const parserFileContext_t& context, const achar* pName )
	: context( context )
	, name( pName )
{}


/*
==================
CEcsStubDefaultFieldValue::CEcsStubDefaultFieldValue
==================
*/
CEcsStubDefaultFieldValue::CEcsStubDefaultFieldValue( const parserFileContext_t& context, const parserFileContext_t& valueContext, const achar* pName, const achar* pValue )
	: CEcsStubBase( context, pName )
	, valueContext( valueContext )
	, value( pValue )
{}


/*
==================
CEcsStubField::CEcsStubField
==================
*/
CEcsStubField::CEcsStubField( const parserFileContext_t& context, const parserFileContext_t& typeContext, const achar* pName, const achar* pType )
	: CEcsStubBase( context, pName )
	, typeContext( typeContext )
	, type( pType )
{}


/*
==================
CEcsStubComponent::CEcsStubComponent
==================
*/
CEcsStubComponent::CEcsStubComponent( const parserFileContext_t& context, const achar* pName )
	: CEcsStubBase( context, pName )
{
	scope.startContext	= context;
	scope.endContext	= context;
}


/*
==================
CEcsStubUsing::CEcsStubUsing
==================
*/
CEcsStubUsing::CEcsStubUsing( const parserFileContext_t& context, const achar* pName )
	: CEcsStubBase( context, pName )
{}


/*
==================
CEcsStubModule::CEcsStubModule
==================
*/
CEcsStubModule::CEcsStubModule( const parserFileContext_t& context, const achar* pName )
	: CEcsStubBase( context, pName )
{
	scope.startContext	= context;
	scope.endContext	= context;
}