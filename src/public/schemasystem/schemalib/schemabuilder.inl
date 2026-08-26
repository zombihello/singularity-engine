#pragma once

/*
==================
CSchemaTypeBuilderBase::CSchemaTypeBuilderBase
==================
*/
FORCEINLINE CSchemaTypeBuilderBase::CSchemaTypeBuilderBase()
	: pName( "" )
	, pAliasName( "" )
	, flags( SCHEMA_TYPE_FLAG_NONE )
{
}

/*
==================
CSchemaTypeBuilderBase::AddMetadata
==================
*/
FORCEINLINE void CSchemaTypeBuilderBase::AddMetadata( const char* pKey, const char* pValue )
{
	metadata.emplace_back( schemaMetadataDesc_t{ pKey, pValue } );
}

/*
==================
CSchemaTypeBuilderBase::SetName
==================
*/
FORCEINLINE void CSchemaTypeBuilderBase::SetName( const char* pName )
{
	CSchemaTypeBuilderBase::pName = pName ? pName : "";
}

/*
==================
CSchemaTypeBuilderBase::SetAliasName
==================
*/
FORCEINLINE void CSchemaTypeBuilderBase::SetAliasName( const char* pAliasName )
{
	CSchemaTypeBuilderBase::pAliasName = pAliasName ? pAliasName : "";
}

/*
==================
CSchemaTypeBuilderBase::SetFlags
==================
*/
FORCEINLINE void CSchemaTypeBuilderBase::SetFlags( uint32 flags )
{
	CSchemaTypeBuilderBase::flags = flags;
}

/*
==================
CSchemaEnumBuilder::CSchemaEnumBuilder
==================
*/
FORCEINLINE CSchemaEnumBuilder::CSchemaEnumBuilder()
	: typeSize( 0 )
	, alignment( 0 )
	, underlyingType( SCHEMA_FIELD_TYPE_NONE )
{
}

/*
==================
CSchemaEnumBuilder::AddValue
==================
*/
FORCEINLINE void CSchemaEnumBuilder::AddValue( const char* pValueName, const char* pValueAliasName, int64 value )
{
	values.emplace_back( schemaEnumValueDesc_t{ pValueName, pValueAliasName ? pValueAliasName : "", value } );
}

/*
==================
CSchemaEnumBuilder::SetSize
==================
*/
FORCEINLINE void CSchemaEnumBuilder::SetSize( size typeSize )
{
	CSchemaEnumBuilder::typeSize = typeSize;
}

/*
==================
CSchemaEnumBuilder::SetAlignment
==================
*/
FORCEINLINE void CSchemaEnumBuilder::SetAlignment( uint32 alignment )
{
	CSchemaEnumBuilder::alignment = alignment;
}

/*
==================
CSchemaEnumBuilder::SetUnderlyingType
==================
*/
FORCEINLINE void CSchemaEnumBuilder::SetUnderlyingType( schemaFieldType_t underlyingType )
{
	CSchemaEnumBuilder::underlyingType = underlyingType;
}

/*
==================
CSchemaInterfaceBuilder::CSchemaInterfaceBuilder
==================
*/
FORCEINLINE CSchemaInterfaceBuilder::CSchemaInterfaceBuilder()
	: pBaseTypeName( "" )
{
}

/*
==================
CSchemaInterfaceBuilder::SetBaseType
==================
*/
FORCEINLINE void CSchemaInterfaceBuilder::SetBaseType( const char* pBaseTypeName )
{
	CSchemaInterfaceBuilder::pBaseTypeName = pBaseTypeName ? pBaseTypeName : "";
}

/*
==================
CSchemaStructBuilder::CSchemaStructBuilder
==================
*/
FORCEINLINE CSchemaStructBuilder::CSchemaStructBuilder()
	: pBaseTypeName( "" )
	, baseOffset( 0 )
	, typeSize( 0 )
	, alignment( 0 )
	, pObjectOps( NULL )
{
}

/*
==================
CSchemaStructBuilder::AddFieldMetadata
==================
*/
FORCEINLINE void CSchemaStructBuilder::AddFieldMetadata( const char* pKey, const char* pValue )
{
	AssertMsg( !fieldsMetadata.empty(), "Add a field metadata without a field before it" );
	fieldsMetadata.back()->emplace_back( schemaMetadataDesc_t{ pKey, pValue } );
}

/*
==================
CSchemaStructBuilder::SetSize
==================
*/
FORCEINLINE void CSchemaStructBuilder::SetSize( size typeSize )
{
	CSchemaStructBuilder::typeSize = typeSize;
}

/*
==================
CSchemaStructBuilder::SetAlignment
==================
*/
FORCEINLINE void CSchemaStructBuilder::SetAlignment( uint32 alignment )
{
	CSchemaStructBuilder::alignment = alignment;
}

/*
==================
CSchemaStructBuilder::SetBaseType
==================
*/
FORCEINLINE void CSchemaStructBuilder::SetBaseType( const char* pBaseTypeName, ptrint baseOffset )
{
	CSchemaStructBuilder::pBaseTypeName = pBaseTypeName ? pBaseTypeName : "";
	CSchemaStructBuilder::baseOffset	= baseOffset;
}

/*
==================
CSchemaStructBuilder::SetObjectOps
==================
*/
FORCEINLINE void CSchemaStructBuilder::SetObjectOps( const schemaObjectOps_t* pObjectOps )
{
	CSchemaStructBuilder::pObjectOps = pObjectOps;
}

/*
==================
CSchemaClassBuilder::CSchemaClassBuilder
==================
*/
FORCEINLINE CSchemaClassBuilder::CSchemaClassBuilder()
	: objectOffset( 0 )
{
}

/*
==================
CSchemaClassBuilder::AddInterface
==================
*/
FORCEINLINE void CSchemaClassBuilder::AddInterface( const char* pTypeName, ptrint offset )
{
	interfaces.emplace_back( schemaImplementedInterfaceDesc_t{ pTypeName, offset } );
}

/*
==================
CSchemaClassBuilder::SetObjectOffset
==================
*/
FORCEINLINE void CSchemaClassBuilder::SetObjectOffset( ptrint objectOffset )
{
	CSchemaClassBuilder::objectOffset = objectOffset;
}

/*
==================
schemaFieldShaper_t::Add
==================
*/
template<typename TMember>
FORCEINLINE void schemaFieldShaper_t<TMember, SCHEMA_MEMBER_SHAPE_SINGLE>::Add( CSchemaStructBuilder& schema, schemaFieldDesc_t& fieldDesc, size offset )
{
	fieldDesc.offset = offset;
	schema.AddField( fieldDesc, NULL );
}

/*
==================
schemaFieldShaper_t::Add
==================
*/
template<typename TMember>
FORCEINLINE void schemaFieldShaper_t<TMember, SCHEMA_MEMBER_SHAPE_FIXED_ARRAY>::Add( CSchemaStructBuilder& schema, schemaFieldDesc_t& fieldDesc, size offset )
{
	schemaFieldDesc_t containerDesc = {};
	containerDesc.pName				= fieldDesc.pName;
	containerDesc.pAliasName		= fieldDesc.pAliasName;
	containerDesc.pTypeName			= "";
	containerDesc.type				= SCHEMA_FIELD_TYPE_FIXED_ARRAY;
	containerDesc.flags				= fieldDesc.flags;
	containerDesc.offset			= offset;
	containerDesc.memberSize		= sizeof( TMember );
	containerDesc.numFixedElements	= (uint32)schemaShapeTraits_t<TMember>::NUM_FIXED_ELEMENTS;
	containerDesc.pCopyFn			= Schema_GetCopyMemberFn<TMember>();
	containerDesc.pContainerOps		= Schema_GetContainerOps<TMember>();
	fieldDesc.offset				= 0;
	schema.AddField( containerDesc, &fieldDesc );
}

/*
==================
schemaFieldShaper_t::Add
==================
*/
template<typename TMember>
FORCEINLINE void schemaFieldShaper_t<TMember, SCHEMA_MEMBER_SHAPE_ARRAY>::Add( CSchemaStructBuilder& schema, schemaFieldDesc_t& fieldDesc, size offset )
{
	schemaFieldDesc_t containerDesc = {};
	containerDesc.pName				= fieldDesc.pName;
	containerDesc.pAliasName		= fieldDesc.pAliasName;
	containerDesc.pTypeName			= "";
	containerDesc.type				= SCHEMA_FIELD_TYPE_ARRAY;
	containerDesc.flags				= fieldDesc.flags;
	containerDesc.offset			= offset;
	containerDesc.memberSize		= sizeof( TMember );
	containerDesc.pCopyFn			= Schema_GetCopyMemberFn<TMember>();
	containerDesc.pContainerOps		= Schema_GetContainerOps<TMember>();
	fieldDesc.offset				= 0;
	schema.AddField( containerDesc, &fieldDesc );
}

/*
==================
Schema_MakeFieldDesc
==================
*/
template<typename TElement>
FORCEINLINE schemaFieldDesc_t Schema_MakeFieldDesc( const char* pName, const char* pAliasName, schemaFieldType_t type, uint32 flags )
{
	schemaFieldDesc_t fieldDesc = {};
	fieldDesc.pName				= pName;
	fieldDesc.pAliasName		= pAliasName;
	fieldDesc.pTypeName			= "";
	fieldDesc.type				= type;
	fieldDesc.flags				= flags;
	fieldDesc.memberSize		= sizeof( TElement );
	fieldDesc.pCopyFn			= Schema_GetCopyMemberFn<TElement>();
	if constexpr ( schemaValueTraits_t<TElement>::TYPE >= SCHEMA_FIELD_TYPE_STRINGID && schemaValueTraits_t<TElement>::TYPE <= SCHEMA_FIELD_TYPE_CSTRING )
	{
		fieldDesc.pStringOps = Schema_GetStringOps<TElement>();
	}

	return fieldDesc;
}

/*
==================
Schema_AddValueField
==================
*/
template<typename TMember>
FORCEINLINE void Schema_AddValueField( CSchemaStructBuilder& schema, const char* pName, const char* pAliasName, size offset, uint32 flags )
{
	using memberInfo_t			= schemaMemberInfo_t<TMember>;
	using element_t				= typename memberInfo_t::element_t;
	schemaFieldDesc_t fieldDesc = Schema_MakeFieldDesc<element_t>( pName, pAliasName, (schemaFieldType_t)memberInfo_t::TYPE, flags );
	fieldDesc.pTypeName			= memberInfo_t::GetTypeName();
	schemaFieldShaper_t<TMember>::Add( schema, fieldDesc, offset );
}

/*
==================
Schema_AddCustomField
==================
*/
template<typename TMember>
FORCEINLINE void Schema_AddCustomField( CSchemaStructBuilder& schema, const char* pName, const char* pAliasName, size offset, uint32 flags, const schemaCustomOps_t* pCustomOps )
{
	using element_t				= typename schemaShapeTraits_t<TMember>::element_t;
	schemaFieldDesc_t fieldDesc = Schema_MakeFieldDesc<element_t>( pName, pAliasName, SCHEMA_FIELD_TYPE_CUSTOM, flags );
	fieldDesc.pCustomOps		= pCustomOps;
	schemaFieldShaper_t<TMember>::Add( schema, fieldDesc, offset );
}
