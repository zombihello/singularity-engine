#pragma once

/*
==================
CStudioAPIBoundShaderStateKeyVk::boundShaderStateKeyHasher_t::operator()
==================
*/
FORCEINLINE std::size_t CStudioAPIBoundShaderStateKeyVk::boundShaderStateKeyHasher_t::operator()( const CStudioAPIBoundShaderStateKeyVk& key ) const
{
	return key.GetHash();
}

/*
==================
CStudioAPIBoundShaderStateKeyVk::GetHash
==================
*/
FORCEINLINE uint64 CStudioAPIBoundShaderStateKeyVk::GetHash() const
{
	return hash;
}

/*
==================
CStudioAPIBoundShaderStateKeyVk::operator<
==================
*/
FORCEINLINE bool CStudioAPIBoundShaderStateKeyVk::operator<( const CStudioAPIBoundShaderStateKeyVk& other ) const
{
	return hash < other.hash;
}

/*
==================
CStudioAPIBoundShaderStateKeyVk::operator==
==================
*/
FORCEINLINE bool CStudioAPIBoundShaderStateKeyVk::operator==( const CStudioAPIBoundShaderStateKeyVk& other ) const
{
	return hash == other.hash;
}

/*
==================
CStudioAPIBoundShaderStateCacheVk::Add
==================
*/
FORCEINLINE void CStudioAPIBoundShaderStateCacheVk::Add( const CStudioAPIBoundShaderStateKeyVk& key, CStudioAPIBoundShaderStateVk* pBoundShaderState )
{
	boundShaderStateDict[key] = pBoundShaderState;
}

/*
==================
CStudioAPIBoundShaderStateCacheVk::Find
==================
*/
FORCEINLINE CStudioAPIBoundShaderStateVk* CStudioAPIBoundShaderStateCacheVk::Find( const CStudioAPIBoundShaderStateKeyVk& key ) const
{
	auto itBoundShaderState = boundShaderStateDict.find( key );
	if ( itBoundShaderState == boundShaderStateDict.end() )
	{
		return NULL;
	}
	return itBoundShaderState->second;
}

/*
==================
CStudioAPIBoundShaderStateCacheVk::Remove
==================
*/
FORCEINLINE void CStudioAPIBoundShaderStateCacheVk::Remove( const CStudioAPIBoundShaderStateKeyVk& key )
{
	boundShaderStateDict.erase( key );
}

/*
==================
CStudioAPIBoundShaderStateCacheVk::RemoveAll
==================
*/
FORCEINLINE void CStudioAPIBoundShaderStateCacheVk::RemoveAll()
{
	boundShaderStateDict.clear();
}