#include "tools/ecscompiler/ecsfieldstubs.h"

// Table for convert from ecsMetadataType_t to text
static const achar* s_pEcsMetadataTypeNames[] =
{
	"serialize",	// ECS_METADATA_TYPE_SERIALIZE
	"name"			// ECS_METADATA_TYPE_NAME
};
static_assert( ARRAYSIZE( s_pEcsMetadataTypeNames ) == ECS_METADATA_NUM_TYPES, "Array size 's_pEcsMetadataTypeNames' must be equal to ECS_METADATA_NUM_TYPES" );


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
CEcsStubMetadataValue::CEcsStubMetadataValue( const parserFileContext_t& context, ecsMetadataType_t type )
	: CEcsStubBase( context, s_pEcsMetadataTypeNames[type])
	, pValueContext( NULL )
	, type( type )
{}

/*
==================
CEcsStubMetadataValue::CEcsStubMetadataValue
==================
*/
CEcsStubMetadataValue::CEcsStubMetadataValue( const parserFileContext_t& context, const parserFileContext_t* pValueContext, ecsMetadataType_t type, const achar* pValue )
	: CEcsStubBase( context, s_pEcsMetadataTypeNames[type] )
	, pValueContext( pValueContext )
	, value( pValue )
	, type( type )
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
{
	// Remove whitespace at begin and at the end
	value.erase( 0, value.find_first_not_of( " \t\n\r\f\v" ) );
	value.erase( value.find_last_not_of( " \t\n\r\f\v" ) + 1 );
}


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
CEcsStubDataType::CEcsStubDataType
==================
*/
CEcsStubDataType::CEcsStubDataType( const parserFileContext_t& context, const achar* pName, CEcsStubMetadata* pMetadata /* = NULL */ )
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