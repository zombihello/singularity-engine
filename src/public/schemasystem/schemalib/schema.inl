#pragma once

/*
==================
schemaTypeCache_t::schemaTypeCache_t
==================
*/
template<class TSchemaType>
FORCEINLINE schemaTypeCache_t<TSchemaType>::schemaTypeCache_t( const char* pName )
	: pName( pName )
{
	pType.store( NULL, eastl::memory_order_release );
	generation.store( 0, eastl::memory_order_release );
}

/*
==================
schemaTypeCache_t::Get
==================
*/
template<class TSchemaType>
FORCEINLINE const TSchemaType* schemaTypeCache_t<TSchemaType>::Get()
{
	// Still use the cached type if the schema system generation same
	PROFILER_SCOPE_FUNC();
	if ( !g_pSchemaSystem )
	{
		return NULL;
	}

	uint32			   curGeneration = g_pSchemaSystem->GetGeneration();
	const TSchemaType* pCachedType	 = pType.load( eastl::memory_order_relaxed );
	if ( pCachedType && generation.load( eastl::memory_order_relaxed ) == curGeneration )
	{
		return pCachedType;
	}

	// Otherwise try find the type in the schema system
	const ISchemaType* pFoundType = g_pSchemaSystem->FindType( pName );
	pCachedType					  = pFoundType ? pFoundType->As<TSchemaType>() : NULL;
	AssertMsg( pCachedType, "'%s' is not described anywhere, its BEGIN_SCHEMA_* is missing", pName );

	pType.store( pCachedType, eastl::memory_order_relaxed );
	generation.store( curGeneration, eastl::memory_order_relaxed );
	return pCachedType;
}

/*
==================
Schema_CastOffset
==================
*/
template<typename TClass, typename TCastTo>
FORCEINLINE ptrint Schema_CastOffset()
{
	// A null pointer cannot be adjusted, so a fake non-null one is used to measure
	// where the subobject sits inside the complete object
	static_assert( eastl::is_base_of<TCastTo, TClass>::value, "The type does not derive from what its offset is being measured to" );
	TClass* pFakeObject = (TClass*)0x1000;
	return (ptrint)( (byte*)(TCastTo*)pFakeObject - (byte*)pFakeObject );
}

/*
==================
schemaBaseInfo_t::Name
==================
*/
template<typename TType, typename TBase>
FORCEINLINE const char* schemaBaseInfo_t<TType, TBase>::Name()
{
	return TBase::GetSchemaTypeNameStatic();
}

/*
==================
schemaBaseInfo_t::Offset
==================
*/
template<typename TType, typename TBase>
FORCEINLINE ptrint schemaBaseInfo_t<TType, TBase>::Offset()
{
	return Schema_CastOffset<TType, TBase>();
}

/*
==================
schemaBaseInfo_t::Name
==================
*/
template<typename TType>
FORCEINLINE const char* schemaBaseInfo_t<TType, void>::Name()
{
	return "";
}

/*
==================
schemaBaseInfo_t::Offset
==================
*/
template<typename TType>
FORCEINLINE ptrint schemaBaseInfo_t<TType, void>::Offset()
{
	return 0;
}

/*
==================
Schema_IsA
==================
*/
template<typename TType>
FORCEINLINE bool Schema_IsA( const ISchemaObject* pObject )
{
	const ISchemaClass* pClassObject = pObject ? pObject->GetSchemaClass() : NULL;
	const ISchemaClass* pSchemaType	 = TType::GetSchemaTypeStatic();
	return pClassObject && pSchemaType && pClassObject->IsA( pSchemaType );
}

/*
==================
Schema_Cast
==================
*/
template<typename TType>
FORCEINLINE TType* Schema_Cast( ISchemaObject* pObject )
{
	const ISchemaClass* pClassObject = pObject ? pObject->GetSchemaClass() : NULL;
	const ISchemaClass* pSchemaType	 = TType::GetSchemaTypeStatic();
	return pClassObject && pSchemaType ? (TType*)pClassObject->Cast( pObject, pSchemaType ) : NULL;
}

/*
==================
Schema_Cast
==================
*/
template<typename TType>
FORCEINLINE const TType* Schema_Cast( const ISchemaObject* pObject )
{
	return Schema_Cast<TType>( (ISchemaObject*)pObject );
}
