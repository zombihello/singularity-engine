#pragma once

/*
==================
schemaMemberCopy_t::Copy
==================
*/
template<typename TMember, bool bIsArray>
FORCEINLINE void schemaMemberCopy_t<TMember, bIsArray>::Copy( void* pDestMember, const void* pSrcMember )
{
	*(TMember*)pDestMember = *(const TMember*)pSrcMember;
}

/*
==================
schemaMemberCopy_t::Copy
==================
*/
template<typename TMember>
FORCEINLINE void schemaMemberCopy_t<TMember, true>::Copy( void* pDestMember, const void* pSrcMember )
{
	TMember&	   destMember = *(TMember*)pDestMember;
	const TMember& srcMember  = *(const TMember*)pSrcMember;
	for ( size index = 0, count = (size)eastl::extent<TMember>::value; index < count; ++index )
	{
		destMember[index] = srcMember[index];
	}
}

/*
==================
schemaStringMemberOps_t<CStringID>::SetText

NOTE: This interns into the string pool of THIS module, which is the module
	  that owns the object - the table is instantiated there. Never serialize
	  a CStringID as its raw id
==================
*/
FORCEINLINE void schemaStringMemberOps_t<CStringID>::SetText( void* pMember, const char* pValue )
{
	*(CStringID*)pMember = pValue ? pValue : "";
}

/*
==================
schemaStringMemberOps_t<CStringID>::GetText
==================
*/
FORCEINLINE const char* schemaStringMemberOps_t<CStringID>::GetText( const void* pMember )
{
	return ( (const CStringID*)pMember )->ToString();
}

/*
==================
schemaStringMemberOps_t<eastl::string>::SetText
==================
*/
FORCEINLINE void schemaStringMemberOps_t<eastl::string>::SetText( void* pMember, const char* pValue )
{
	*(eastl::string*)pMember = pValue ? pValue : "";
}

/*
==================
schemaStringMemberOps_t<eastl::string>::GetText
==================
*/
FORCEINLINE const char* schemaStringMemberOps_t<eastl::string>::GetText( const void* pMember )
{
	return ( (const eastl::string*)pMember )->c_str();
}

/*
==================
schemaStringMemberOps_t<const char*>::SetText
==================
*/
FORCEINLINE void schemaStringMemberOps_t<const char*>::SetText( void* pMember, const char* pValue )
{
	AssertMsg( false, "A c string member cannot be written through the schema, use eastl::string or CStringID" );
}

/*
==================
schemaStringMemberOps_t<const char*>::GetText
==================
*/
FORCEINLINE const char* schemaStringMemberOps_t<const char*>::GetText( const void* pMember )
{
	const char* pValue = *(const char* const*)pMember;
	return pValue ? pValue : "";
}

/*
==================
schemaContainerMemberOps_t::SetNum
==================
*/
template<typename TMember>
FORCEINLINE bool schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_FIXED_ARRAY>::SetNum( void* pMember, uint32 count )
{
	return count == (uint32)NUM_FIXED_ELEMENTS;
}

/*
==================
schemaContainerMemberOps_t::Insert
==================
*/
template<typename TMember>
FORCEINLINE bool schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_FIXED_ARRAY>::Insert( void* pMember, uint32 index )
{
	return false;
}

/*
==================
schemaContainerMemberOps_t::Remove
==================
*/
template<typename TMember>
FORCEINLINE bool schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_FIXED_ARRAY>::Remove( void* pMember, uint32 index )
{
	return false;
}

/*
==================
schemaContainerMemberOps_t::Clear
==================
*/
template<typename TMember>
FORCEINLINE void schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_FIXED_ARRAY>::Clear( void* pMember )
{
}

/*
==================
schemaContainerMemberOps_t::GetNum
==================
*/
template<typename TMember>
FORCEINLINE uint32 schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_FIXED_ARRAY>::GetNum( const void* pMember )
{
	return (uint32)NUM_FIXED_ELEMENTS;
}

/*
==================
schemaContainerMemberOps_t::GetElementPtr
==================
*/
template<typename TMember>
FORCEINLINE void* schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_FIXED_ARRAY>::GetElementPtr( void* pMember, uint32 index )
{
	TMember& member = *(TMember*)pMember;
	return index < (uint32)NUM_FIXED_ELEMENTS ? &member[index] : NULL;
}

/*
==================
schemaContainerMemberOps_t::SetNum
==================
*/
template<typename TMember>
FORCEINLINE bool schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_ARRAY>::SetNum( void* pMember, uint32 count )
{
	( (TMember*)pMember )->resize( count );
	return true;
}

/*
==================
schemaContainerMemberOps_t::Insert
==================
*/
template<typename TMember>
FORCEINLINE bool schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_ARRAY>::Insert( void* pMember, uint32 index )
{
	TMember& member = *(TMember*)pMember;
	if ( index > (uint32)member.size() )
	{
		return false;
	}

	member.insert( member.begin() + index, element_t() );
	return true;
}

/*
==================
schemaContainerMemberOps_t::Remove
==================
*/
template<typename TMember>
FORCEINLINE bool schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_ARRAY>::Remove( void* pMember, uint32 index )
{
	TMember& member = *(TMember*)pMember;
	if ( index >= (uint32)member.size() )
	{
		return false;
	}

	member.erase( member.begin() + index );
	return true;
}

/*
==================
schemaContainerMemberOps_t::Clear
==================
*/
template<typename TMember>
FORCEINLINE void schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_ARRAY>::Clear( void* pMember )
{
	( (TMember*)pMember )->clear();
}

/*
==================
schemaContainerMemberOps_t::GetNum
==================
*/
template<typename TMember>
FORCEINLINE uint32 schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_ARRAY>::GetNum( const void* pMember )
{
	return (uint32)( (const TMember*)pMember )->size();
}

/*
==================
schemaContainerMemberOps_t::GetElementPtr
==================
*/
template<typename TMember>
FORCEINLINE void* schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_ARRAY>::GetElementPtr( void* pMember, uint32 index )
{
	TMember& member = *(TMember*)pMember;
	return index < (uint32)member.size() ? &member[index] : NULL;
}

/*
==================
schemaObjectMemberOps_t::Construct
==================
*/
template<typename TType, bool bCanConstruct>
FORCEINLINE void schemaObjectMemberOps_t<TType, bCanConstruct>::Construct( void* pMemory )
{
	Assert( pMemory );
	new ( pMemory ) TType();
}

/*
==================
schemaObjectMemberOps_t::Destruct
==================
*/
template<typename TType, bool bCanConstruct>
FORCEINLINE void schemaObjectMemberOps_t<TType, bCanConstruct>::Destruct( void* pObject )
{
	Assert( pObject );
	( (TType*)pObject )->~TType();
}

/*
==================
schemaObjectMemberOps_t::New
==================
*/
template<typename TType, bool bCanConstruct>
FORCEINLINE void* schemaObjectMemberOps_t<TType, bCanConstruct>::New()
{
	return new TType();
}

/*
==================
schemaObjectMemberOps_t::Delete
==================
*/
template<typename TType, bool bCanConstruct>
FORCEINLINE void schemaObjectMemberOps_t<TType, bCanConstruct>::Delete( void* pObject )
{
	delete (TType*)pObject;
}

/*
==================
schemaObjectMemberOps_t::Construct
==================
*/
template<typename TType>
FORCEINLINE void schemaObjectMemberOps_t<TType, false>::Construct( void* pMemory )
{
	AssertMsg( false, "'%s' is abstract or has no default constructor", TType::GetSchemaTypeNameStatic() );
}

/*
==================
schemaObjectMemberOps_t::Destruct
==================
*/
template<typename TType>
FORCEINLINE void schemaObjectMemberOps_t<TType, false>::Destruct( void* pObject )
{
	AssertMsg( false, "'%s' is abstract or has no default constructor", TType::GetSchemaTypeNameStatic() );
}

/*
==================
schemaObjectMemberOps_t::New
==================
*/
template<typename TType>
FORCEINLINE void* schemaObjectMemberOps_t<TType, false>::New()
{
	AssertMsg( false, "'%s' is abstract or has no default constructor", TType::GetSchemaTypeNameStatic() );
	return NULL;
}

/*
==================
schemaObjectMemberOps_t::Delete
==================
*/
template<typename TType>
FORCEINLINE void schemaObjectMemberOps_t<TType, false>::Delete( void* pObject )
{
	AssertMsg( false, "'%s' is abstract or has no default constructor", TType::GetSchemaTypeNameStatic() );
}

/*
==================
Schema_GetCopyMemberFn
==================
*/
template<typename TMember>
FORCEINLINE schemaFieldDesc_t::copyMemberFn_t Schema_GetCopyMemberFn()
{
	// A trivially copyable member needs nobody called back for it - the schema
	// system copies it by its size
	return !eastl::is_trivially_copyable<TMember>::value ? &schemaMemberCopy_t<TMember>::Copy : NULL;
}

/*
==================
Schema_GetStringOps
==================
*/
template<typename TMember>
FORCEINLINE const schemaStringOps_t* Schema_GetStringOps()
{
	using stringOps_t						   = schemaStringMemberOps_t<TMember>;
	static const schemaStringOps_t s_stringOps = {
		&stringOps_t::SetText,
		&stringOps_t::GetText
	};
	return &s_stringOps;
}

/*
==================
Schema_GetContainerOps
==================
*/
template<typename TMember>
FORCEINLINE const schemaContainerOps_t* Schema_GetContainerOps()
{
	using containerOps_t							 = schemaContainerMemberOps_t<TMember>;
	static const schemaContainerOps_t s_containerOps = {
		&containerOps_t::Insert,
		&containerOps_t::Remove,
		&containerOps_t::Clear,
		&containerOps_t::SetNum,
		&containerOps_t::GetNum,
		&containerOps_t::GetElementPtr
	};
	return &s_containerOps;
}

/*
==================
Schema_GetObjectOps
==================
*/
template<typename TType>
FORCEINLINE const schemaObjectOps_t* Schema_GetObjectOps()
{
	using objectOps_t						   = schemaObjectMemberOps_t<TType>;
	static const schemaObjectOps_t s_objectOps = {
		&objectOps_t::Construct,
		&objectOps_t::Destruct,
		&objectOps_t::New,
		&objectOps_t::Delete
	};
	return &s_objectOps;
}
