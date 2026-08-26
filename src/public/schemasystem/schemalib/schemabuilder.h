#pragma once
#include "schemasystem/ischemasystem.h"
#include "schemasystem/schemalib/schemaops.h"

//-----------------------------------------------------------------------------
// Putting one type descriptor together and handing it to the schema system
//-----------------------------------------------------------------------------
class CSchemaTypeBuilderBase
{
public:
	void AddMetadata( const char* pKey, const char* pValue );

	void SetName( const char* pName );
	void SetAliasName( const char* pAliasName );
	void SetFlags( uint32 flags );

protected:
	CSchemaTypeBuilderBase();

	const char*							pName;
	const char*							pAliasName;
	uint32								flags;
	eastl::vector<schemaMetadataDesc_t> metadata;
};

class CSchemaEnumBuilder : public CSchemaTypeBuilderBase
{
public:
	CSchemaEnumBuilder();

	void AddValue( const char* pValueName, const char* pValueAliasName, int64 value );
	void Register( schemaScopeId_t scopeId );

	void SetSize( size typeSize );
	void SetAlignment( uint32 alignment );
	void SetUnderlyingType( schemaFieldType_t underlyingType );

private:
	size								 typeSize;
	uint32								 alignment;
	schemaFieldType_t					 underlyingType;
	eastl::vector<schemaEnumValueDesc_t> values;
};

class CSchemaInterfaceBuilder : public CSchemaTypeBuilderBase
{
public:
	CSchemaInterfaceBuilder();

	void Register( schemaScopeId_t scopeId );
	void SetBaseType( const char* pBaseTypeName );

private:
	const char* pBaseTypeName;
};

class CSchemaStructBuilder : public CSchemaTypeBuilderBase
{
public:
	CSchemaStructBuilder();
	~CSchemaStructBuilder();

	void AddField( const schemaFieldDesc_t& fieldDesc, const schemaFieldDesc_t* pInnerFieldDesc = NULL );  // Takes a field, and the descriptor of its element when it is a container
	void AddFieldMetadata( const char* pKey, const char* pValue );										   // Attaches a metadata to the field that was described last
	void Register( schemaScopeId_t scopeId );

	void SetSize( size typeSize );
	void SetAlignment( uint32 alignment );
	void SetBaseType( const char* pBaseTypeName, ptrint baseOffset );
	void SetObjectOps( const schemaObjectOps_t* pObjectOps );

protected:
	void FillStructDesc( schemaStructDesc_t& structDesc );

	const char*											pBaseTypeName;
	ptrint												baseOffset;
	size												typeSize;
	uint32												alignment;
	const schemaObjectOps_t*							pObjectOps;
	eastl::vector<schemaFieldDesc_t>					fields;
	eastl::vector<eastl::vector<schemaMetadataDesc_t>*> fieldsMetadata;	 // Fields metadata, one list per field
	eastl::vector<schemaFieldDesc_t*>					innerFields;	 // The element descriptors of the container fields
};

class CSchemaClassBuilder : public CSchemaStructBuilder
{
public:
	CSchemaClassBuilder();

	void AddInterface( const char* pTypeName, ptrint offset );	// Adds an interface a class implements, and the adjustment casting to it needs
	void Register( schemaScopeId_t scopeId );

	void SetObjectOffset( ptrint objectOffset );

private:
	ptrint											objectOffset;
	eastl::vector<schemaImplementedInterfaceDesc_t> interfaces;
};

//-----------------------------------------------------------------------------
// Handing one described member to the builder
//
// NOTE: A member holding ONE value simply IS its element field. A member holding
//		 several gets a container over an element field placed at offset zero
//-----------------------------------------------------------------------------
template<typename TMember, uint32 shape = schemaShapeTraits_t<TMember>::SHAPE>
struct schemaFieldShaper_t;

template<typename TMember>
struct schemaFieldShaper_t<TMember, SCHEMA_MEMBER_SHAPE_SINGLE>
{
	static void Add( CSchemaStructBuilder& schema, schemaFieldDesc_t& fieldDesc, size offset );
};

template<typename TMember>
struct schemaFieldShaper_t<TMember, SCHEMA_MEMBER_SHAPE_FIXED_ARRAY>
{
	static void Add( CSchemaStructBuilder& schema, schemaFieldDesc_t& fieldDesc, size offset );
};

template<typename TMember>
struct schemaFieldShaper_t<TMember, SCHEMA_MEMBER_SHAPE_ARRAY>
{
	static void Add( CSchemaStructBuilder& schema, schemaFieldDesc_t& fieldDesc, size offset );
};

//-----------------------------------------------------------------------------
// What the `SCHEMA_FIELD` macros expand into
//-----------------------------------------------------------------------------
template<typename TElement>
schemaFieldDesc_t Schema_MakeFieldDesc( const char* pName, const char* pAliasName, schemaFieldType_t type, uint32 flags );
template<typename TMember>
void Schema_AddValueField( CSchemaStructBuilder& schema, const char* pName, const char* pAliasName, size offset, uint32 flags );
template<typename TMember>
void Schema_AddCustomField( CSchemaStructBuilder& schema, const char* pName, const char* pAliasName, size offset, uint32 flags, const schemaCustomOps_t* pCustomOps );

#include "schemasystem/schemalib/schemabuilder.inl"
