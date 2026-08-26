#pragma once
#include "tier0/assert.h"
#include "schemasystem/schemadesc.h"
#include "schemasystem/schemalib/schemavalue.h"

//-----------------------------------------------------------------------------
// Building the operation tables a described type hands over
//-----------------------------------------------------------------------------
template<typename TMember, bool bIsArray = eastl::is_array<TMember>::value>
struct schemaMemberCopy_t
{
	static void Copy( void* pDestMember, const void* pSrcMember );
};

template<typename TMember>
struct schemaMemberCopy_t<TMember, true>
{
	static void Copy( void* pDestMember, const void* pSrcMember );
};

//-----------------------------------------------------------------------------
// A `CStringID`, an `eastl::string` or a `const char*` member
//-----------------------------------------------------------------------------
template<typename TMember>
struct schemaStringMemberOps_t;

template<>
struct schemaStringMemberOps_t<CStringID>
{
	static void		   SetText( void* pMember, const char* pValue );
	static const char* GetText( const void* pMember );
};

template<>
struct schemaStringMemberOps_t<eastl::string>
{
	static void		   SetText( void* pMember, const char* pValue );
	static const char* GetText( const void* pMember );
};

template<>
struct schemaStringMemberOps_t<const char*>
{
	static void		   SetText( void* pMember, const char* pValue );
	static const char* GetText( const void* pMember );
};

//-----------------------------------------------------------------------------
// A `Type[N]` or an `eastl::vector<Type>` member
//-----------------------------------------------------------------------------
template<typename TMember, uint32 shape = schemaShapeTraits_t<TMember>::SHAPE>
struct schemaContainerMemberOps_t;

// NOTE: The count is part of the type, so every resize is refused and clearing
// does nothing
template<typename TMember>
struct schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_FIXED_ARRAY>
{
	using element_t = typename schemaShapeTraits_t<TMember>::element_t;
	enum
	{
		NUM_FIXED_ELEMENTS = schemaShapeTraits_t<TMember>::NUM_FIXED_ELEMENTS
	};

	static bool	  SetNum( void* pMember, uint32 count );
	static bool	  Insert( void* pMember, uint32 index );
	static bool	  Remove( void* pMember, uint32 index );
	static void	  Clear( void* pMember );
	static uint32 GetNum( const void* pMember );
	static void*  GetElementPtr( void* pMember, uint32 index );
};

template<typename TMember>
struct schemaContainerMemberOps_t<TMember, SCHEMA_MEMBER_SHAPE_ARRAY>
{
	using element_t = typename schemaShapeTraits_t<TMember>::element_t;

	static bool	  SetNum( void* pMember, uint32 count );
	static bool	  Insert( void* pMember, uint32 index );
	static bool	  Remove( void* pMember, uint32 index );
	static void	  Clear( void* pMember );
	static uint32 GetNum( const void* pMember );
	static void*  GetElementPtr( void* pMember, uint32 index );
};

//-----------------------------------------------------------------------------
// Building and tearing down an object of a described type
//
// NOTE: An abstract or a non default constructible type cannot be built, in that
//		 case the operation asserts and does nothing instead of failing to compile
//-----------------------------------------------------------------------------
template<typename TType, bool bCanConstruct = !eastl::is_abstract<TType>::value && eastl::is_default_constructible<TType>::value>
struct schemaObjectMemberOps_t
{
	enum
	{
		CAN_CONSTRUCT = 1
	};

	static void	 Construct( void* pMemory );
	static void	 Destruct( void* pObject );
	static void* New();
	static void	 Delete( void* pObject );
};

template<typename TType>
struct schemaObjectMemberOps_t<TType, false>
{
	enum
	{
		CAN_CONSTRUCT = 0
	};

	static void	 Construct( void* pMemory );
	static void	 Destruct( void* pObject );
	static void* New();
	static void	 Delete( void* pObject );
};

//-----------------------------------------------------------------------------
// The tables themselves
//-----------------------------------------------------------------------------
template<typename TMember>
schemaFieldDesc_t::copyMemberFn_t Schema_GetCopyMemberFn();
template<typename TMember>
const schemaStringOps_t* Schema_GetStringOps();
template<typename TMember>
const schemaContainerOps_t* Schema_GetContainerOps();
template<typename TType>
const schemaObjectOps_t* Schema_GetObjectOps();

#include "schemasystem/schemalib/schemaops.inl"
