#pragma once

/*
==================
CStringPool::~CStringPool
==================
*/
template<typename TIdType>
FORCEINLINE CStringPool<TIdType>::~CStringPool()
{
	for ( size index = 0, count = strings.size(); index < count; ++index )
	{
		Mem_Free( (void*)strings[index] );
	}
}

/*
==================
CStringPool::Find
==================
*/
template<typename TIdType>
FORCEINLINE TIdType CStringPool<TIdType>::Find( const char* pString, uint64 length ) const
{
	CScopeLock scopeLock( mutex );
	auto	   it = storageDict.find( eastl::string_view( pString, length ) );
	return it != storageDict.end() ? it->second : (TIdType)INVALID_INDEX;
}

/*
==================
CStringPool::FindOrAdd
==================
*/
template<typename TIdType>
FORCEINLINE TIdType CStringPool<TIdType>::FindOrAdd( const char* pString, uint64 length )
{
	CScopeLock scopeLock( mutex );
	TIdType	   id = Find( pString, length );
	if ( id != (TIdType)INVALID_INDEX )
	{
		return id;
	}

	size numStrings = strings.size();
	if ( numStrings > S_MaxValue<TIdType>() )
	{
		Sys_Error( "String pool overflow, too many strings" );
	}

	id					= (TIdType)numStrings;
	char* pStringInPool = (char*)Mem_Malloc( length + 1 );
	S_Strncpy( pStringInPool, pString, length );
	pStringInPool[length] = '\0';

	strings.emplace_back( pStringInPool );
	storageDict.insert( eastl::make_pair( eastl::move( eastl::string_view( pStringInPool, length ) ), id ) );
	return id;
}

/*
==================
CStringPool::GetString
==================
*/
template<typename TIdType>
FORCEINLINE const char* CStringPool<TIdType>::GetString( TIdType id ) const
{
	CScopeLock scopeLock( mutex );
	return strings[id];
}
