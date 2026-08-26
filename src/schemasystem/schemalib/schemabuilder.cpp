#include "pch_schemalib.h"
#include "schemasystem/schemalib/schemabuilder.h"

/*
==================
CSchemaEnumBuilder::Register
==================
*/
void CSchemaEnumBuilder::Register( schemaScopeId_t scopeId )
{
	PROFILER_SCOPE_FUNC();
	Assert( g_pSchemaSystem );
	schemaEnumDesc_t enumDesc = {};
	enumDesc.pName			  = pName;
	enumDesc.pAliasName		  = pAliasName;
	enumDesc.typeSize		  = typeSize;
	enumDesc.underlyingType	  = underlyingType;
	enumDesc.alignment		  = alignment;
	enumDesc.flags			  = flags;
	enumDesc.numValues		  = (uint32)values.size();
	enumDesc.numMetadata	  = (uint32)metadata.size();
	enumDesc.pValues		  = values.data();
	enumDesc.pMetadata		  = metadata.data();
	g_pSchemaSystem->RegisterEnum( enumDesc, scopeId );
}

/*
==================
CSchemaInterfaceBuilder::Register
==================
*/
void CSchemaInterfaceBuilder::Register( schemaScopeId_t scopeId )
{
	PROFILER_SCOPE_FUNC();
	Assert( g_pSchemaSystem );
	schemaInterfaceDesc_t interfaceDesc = {};
	interfaceDesc.pName					= pName;
	interfaceDesc.pAliasName			= pAliasName;
	interfaceDesc.pBaseTypeName			= pBaseTypeName;
	interfaceDesc.flags					= flags;
	interfaceDesc.numMetadata			= (uint32)metadata.size();
	interfaceDesc.pMetadata				= metadata.data();
	g_pSchemaSystem->RegisterInterface( interfaceDesc, scopeId );
}

/*
==================
CSchemaStructBuilder::~CSchemaStructBuilder
==================
*/
CSchemaStructBuilder::~CSchemaStructBuilder()
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)fieldsMetadata.size(); index < count; ++index )
	{
		delete fieldsMetadata[index];
	}
	for ( uint32 index = 0, count = (uint32)innerFields.size(); index < count; ++index )
	{
		delete innerFields[index];
	}

	fieldsMetadata.clear();
	innerFields.clear();
}

/*
==================
CSchemaStructBuilder::AddField
==================
*/
void CSchemaStructBuilder::AddField( const schemaFieldDesc_t& fieldDesc, const schemaFieldDesc_t* pInnerFieldDesc /* = NULL */ )
{
	PROFILER_SCOPE_FUNC();
	fields.emplace_back( fieldDesc );
	fieldsMetadata.emplace_back( new eastl::vector<schemaMetadataDesc_t>() );
	if ( pInnerFieldDesc )
	{
		innerFields.emplace_back( new schemaFieldDesc_t( *pInnerFieldDesc ) );
		fields.back().pInnerField = innerFields.back();
	}
}

/*
==================
CSchemaStructBuilder::Register
==================
*/
void CSchemaStructBuilder::Register( schemaScopeId_t scopeId )
{
	PROFILER_SCOPE_FUNC();
	Assert( g_pSchemaSystem );
	schemaStructDesc_t structDesc = {};
	FillStructDesc( structDesc );
	g_pSchemaSystem->RegisterStruct( structDesc, scopeId );
}

/*
==================
CSchemaStructBuilder::FillStructDesc
==================
*/
void CSchemaStructBuilder::FillStructDesc( schemaStructDesc_t& structDesc )
{
	// Link fields metadata
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)fields.size(); index < count; ++index )
	{
		const eastl::vector<schemaMetadataDesc_t>& fieldMetadata = *fieldsMetadata[index];
		if ( !fieldMetadata.empty() )
		{
			schemaFieldDesc_t& field = fields[index];
			field.pMetadata			 = fieldMetadata.data();
			field.numMetadata		 = (uint32)fieldMetadata.size();
		}
	}

	// Fill the struct descriptor
	structDesc.pName		 = pName;
	structDesc.pAliasName	 = pAliasName;
	structDesc.pBaseTypeName = pBaseTypeName;
	structDesc.typeSize		 = typeSize;
	structDesc.baseOffset	 = baseOffset;
	structDesc.alignment	 = alignment;
	structDesc.flags		 = flags;
	structDesc.numFields	 = (uint32)fields.size();
	structDesc.numMetadata	 = (uint32)metadata.size();
	structDesc.pFields		 = fields.data();
	structDesc.pMetadata	 = metadata.data();
	structDesc.pObjectOps	 = pObjectOps;
}

/*
==================
CSchemaClassBuilder::Register
==================
*/
void CSchemaClassBuilder::Register( schemaScopeId_t scopeId )
{
	PROFILER_SCOPE_FUNC();
	Assert( g_pSchemaSystem );

	schemaClassDesc_t classDesc = {};
	classDesc.objectOffset		= objectOffset;
	classDesc.pInterfaces		= interfaces.data();
	classDesc.numInterfaces		= (uint32)interfaces.size();
	FillStructDesc( classDesc );
	g_pSchemaSystem->RegisterClass( classDesc, scopeId );
}
