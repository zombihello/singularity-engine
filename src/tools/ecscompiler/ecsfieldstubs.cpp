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
CEcsStubMetadataValue::CEcsStubMetadataValue
==================
*/
CEcsStubMetadataValue::CEcsStubMetadataValue( const parserFileContext_t& context, const parserFileContext_t* pValueContext, const achar* pName, const achar* pValue )
	: CEcsStubBase( context, pName )
	, pValueContext( pValueContext )
	, value( pValue )
{}


/*
==================
CEcsStubMetadata::CEcsStubMetadata
==================
*/
CEcsStubMetadata::CEcsStubMetadata( const parserFileContext_t& context )
	: CEcsStubBase( context, "" )
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
CEcsStubField::CEcsStubField( const parserFileContext_t& context, const parserFileContext_t& typeContext, const achar* pName, const achar* pType, CEcsStubMetadata* pMetadata /* = NULL */ )
	: CEcsStubBase( context, pName )
	, typeContext( typeContext )
	, type( pType )
	, pMetadata( pMetadata )
{}


/*
==================
CEcsStubComponent::CEcsStubComponent
==================
*/
CEcsStubComponent::CEcsStubComponent( const parserFileContext_t& context, const achar* pName, CEcsStubMetadata* pMetadata /* = NULL */ )
	: CEcsStubBase( context, pName )
	, pMetadata( pMetadata )
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
CEcsStubSystemFilter::CEcsStubSystemFilter
==================
*/
CEcsStubSystemFilter::CEcsStubSystemFilter( const parserFileContext_t& context, const achar* pName )
	: CEcsStubBase( context, pName )
{}


/*
==================
CEcsStubSystem::CEcsStubSystem
==================
*/
CEcsStubSystem::CEcsStubSystem( const parserFileContext_t& context, const achar* pName, CEcsStubMetadata* pMetadata /* = NULL */ )
	: CEcsStubBase( context, pName )
	, stage( ECS_SYSTEM_STAGE_ONUPDATE )
	, pMetadata( pMetadata )
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