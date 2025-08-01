#pragma once

/*
==================
CShaderCacheDoc::SetType
==================
*/
FORCEINLINE void CShaderCacheDoc::SetType( studioAPIShaderType_t type )
{
	CShaderCacheDoc::type = type;
}

/*
==================
CShaderCacheDoc::GetType
==================
*/
FORCEINLINE studioAPIShaderType_t CShaderCacheDoc::GetType() const
{
	return type;
}

/*
==================
CShaderCacheDoc::AddCache
==================
*/
FORCEINLINE void CShaderCacheDoc::AddCache( const std::string& entryPointName, const std::vector<byte>& reflectionData, const std::vector<byte>& bytecode )
{
	caches.emplace_back( entryPointName, reflectionData, bytecode );
}

/*
==================
CShaderCacheDoc::GetNumCaches
==================
*/
FORCEINLINE uint64 CShaderCacheDoc::GetNumCaches() const
{
	return ( uint64 )caches.size();
}

/*
==================
CShaderCacheDoc::GetCache
==================
*/
FORCEINLINE const CShaderCacheDoc::shaderCache_t& CShaderCacheDoc::GetCache( uint64 cacheId ) const
{
	Assert( cacheId < ( uint64 )caches.size() );
	return caches[cacheId];
}
