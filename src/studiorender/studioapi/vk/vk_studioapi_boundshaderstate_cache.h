#pragma once
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPIVertexDeclarationVk;
class CStudioAPIVertexShaderVk;
class CStudioAPIPixelShaderVk;
class CStudioAPIHullShaderVk;
class CStudioAPIDomainShaderVk;
class CStudioAPIGeometryShaderVk;
class CStudioAPIBoundShaderStateVk;


//-----------------------------------------------------------------------------
// Key used to map a set of unique decl/vs/ps combinations to a vertex shader resource
//-----------------------------------------------------------------------------
class CStudioAPIBoundShaderStateKeyVk
{
public:
	// Key hasher for using CStudioAPIBoundShaderStateKeyVk in std::unordered_map
	struct boundShaderStateKeyHasher_t
	{
		std::size_t operator()( const CStudioAPIBoundShaderStateKeyVk& key ) const;
	};

	CStudioAPIBoundShaderStateKeyVk( CStudioAPIVertexDeclarationVk* pVertexDeclaration, CStudioAPIVertexShaderVk* pVertexShader, CStudioAPIPixelShaderVk* pPixelShader, CStudioAPIHullShaderVk* pHullShader = NULL, CStudioAPIDomainShaderVk* pDomainShader = NULL, CStudioAPIGeometryShaderVk* pGeometryShader = NULL );

	uint64 GetHash() const;
	bool operator<( const CStudioAPIBoundShaderStateKeyVk& other ) const;
	bool operator==( const CStudioAPIBoundShaderStateKeyVk& other ) const;

public:
	uint64		hash;
};


//-----------------------------------------------------------------------------
// A list of the most recently used bound shader states
//-----------------------------------------------------------------------------
class CStudioAPIBoundShaderStateCacheVk
{
public:
	void Add( const CStudioAPIBoundShaderStateKeyVk& key, CStudioAPIBoundShaderStateVk* pBoundShaderState );
	CStudioAPIBoundShaderStateVk* Find( const CStudioAPIBoundShaderStateKeyVk& key ) const;
	void Remove( const CStudioAPIBoundShaderStateKeyVk& key );
	void RemoveAll();

private:
	std::unordered_map<CStudioAPIBoundShaderStateKeyVk, CStudioAPIBoundShaderStateVk*, CStudioAPIBoundShaderStateKeyVk::boundShaderStateKeyHasher_t>		boundShaderStateDict;
};

#include "studiorender/studioapi/vk/vk_studioapi_boundshaderstate_cache.inl"