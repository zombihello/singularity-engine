#pragma once
#include "tier0/types.h"
#include "schemasystem/schematypes.h"

//-----------------------------------------------------------------------------
// Operations over a described member that MUST run in the module owning the type
//-----------------------------------------------------------------------------
// A `CStringID`, an `eastl::string` or a `const char*` member
struct schemaStringOps_t
{
	using setTextFn_t = void ( * )( void* pMember, const char* pValue );
	using getTextFn_t = const char* (*)( const void* pMember );

	setTextFn_t pSetTextFn;
	getTextFn_t pGetTextFn;
};

// A `TType[N]` or an `eastl::vector<TType>` member
struct schemaContainerOps_t
{
	using insertFn_t		= bool ( * )( void* pMember, uint32 index );
	using removeFn_t		= bool ( * )( void* pMember, uint32 index );
	using clearFn_t			= void ( * )( void* pMember );
	using setNumFn_t		= bool ( * )( void* pMember, uint32 count );
	using getNumFn_t		= uint32 ( * )( const void* pMember );
	using getElementPtrFn_t = void* (*)( void* pMember, uint32 index );

	insertFn_t		  pInsertFn;
	removeFn_t		  pRemoveFn;
	clearFn_t		  pClearFn;
	setNumFn_t		  pSetNumFn;
	getNumFn_t		  pGetNumFn;
	getElementPtrFn_t pGetElementPtrFn;
};

// Building and tearing down an object of a described type
struct schemaObjectOps_t
{
	using constructFn_t = void ( * )( void* pMemory );
	using destructFn_t	= void ( * )( void* pObject );
	using newFn_t		= void* (*)();
	using deleteFn_t	= void ( * )( void* pObject );

	constructFn_t pConstructFn;
	destructFn_t  pDestructFn;
	newFn_t		  pNewFn;
	deleteFn_t	  pDeleteFn;
};

// Operations over a value the schema macros cannot derive on their own
struct schemaCustomOps_t
{
	using readValueFn_t	 = bool ( * )( void* pValue, ISchemaDataReader* pReader );
	using writeValueFn_t = bool ( * )( const void* pValue, ISchemaDataWriter* pWriter );
	using compareFn_t	 = bool ( * )( const void* pLeftValue, const void* pRightValue );

	readValueFn_t  pReadValueFn;
	writeValueFn_t pWriteValueFn;
	compareFn_t	   pCompareFn;
};

//-----------------------------------------------------------------------------
// How a module describes its types to the schema system
//-----------------------------------------------------------------------------
// A metadata for a described type
struct schemaMetadataDesc_t
{
	const char* pKey;
	const char* pValue;
};

// One described field
struct schemaFieldDesc_t
{
	using copyMemberFn_t = void ( * )( void* pDestMember, const void* pSrcMember );

	const char*					pName;
	const char*					pAliasName;
	const char*					pTypeName;	// The type the field points at (only for `SCHEMA_FIELD_TYPE_ENUM`/`STRUCT`/`OBJECTPTR`)
	schemaFieldType_t			type;
	uint32						flags;	// see schemaFieldFlag_t
	size						offset;
	size						memberSize;
	uint32						numFixedElements;  // Only for `SCHEMA_FIELD_TYPE_FIXED_ARRAY`
	uint32						numMetadata;
	const schemaMetadataDesc_t* pMetadata;

	copyMemberFn_t				pCopyFn;		// NULL when the member is trivial, it is then copied by its size
	const schemaStringOps_t*	pStringOps;		// Only for `SCHEMA_FIELD_TYPE_STRINGID`/`STRING`/`CSTRING`
	const schemaContainerOps_t* pContainerOps;	// Only for `SCHEMA_FIELD_TYPE_FIXED_ARRAY`/`ARRAY`
	const schemaCustomOps_t*	pCustomOps;		// Only for `SCHEMA_FIELD_TYPE_CUSTOM`
	const schemaFieldDesc_t*	pInnerField;	// The element in the array, only for `SCHEMA_FIELD_TYPE_FIXED_ARRAY`/`ARRAY`
};

// A described enum
struct schemaEnumValueDesc_t
{
	const char* pName;
	const char* pAliasName;
	int64		value;
};

struct schemaEnumDesc_t
{
	const char*					 pName;
	const char*					 pAliasName;
	size						 typeSize;
	schemaFieldType_t			 underlyingType;
	uint32						 alignment;
	uint32						 flags;	 // see schemaTypeFlag_t
	uint32						 numValues;
	uint32						 numMetadata;
	const schemaEnumValueDesc_t* pValues;
	const schemaMetadataDesc_t*	 pMetadata;
};

// A described interface
struct schemaInterfaceDesc_t
{
	const char*					pName;
	const char*					pAliasName;
	const char*					pBaseTypeName;	// "" when there is no base
	uint32						flags;			// see schemaTypeFlag_t
	uint32						numMetadata;
	const schemaMetadataDesc_t* pMetadata;
};

// An interface a class implements, and how far into the object it sits
struct schemaImplementedInterfaceDesc_t
{
	const char* pTypeName;
	ptrint		offset;
};

// A described struct
struct schemaStructDesc_t
{
	const char*					pName;
	const char*					pAliasName;
	const char*					pBaseTypeName;	// "" when there is no base
	size						typeSize;
	ptrint						baseOffset;
	uint32						alignment;
	uint32						flags;	// see schemaTypeFlag_t
	uint32						numFields;
	uint32						numMetadata;
	const schemaFieldDesc_t*	pFields;
	const schemaMetadataDesc_t* pMetadata;
	const schemaObjectOps_t*	pObjectOps;
};

// A described class
struct schemaClassDesc_t : public schemaStructDesc_t
{
	ptrint									objectOffset;  // How far into the type its `ISchemaObject` sits
	const schemaImplementedInterfaceDesc_t* pInterfaces;
	uint32									numInterfaces;
};
