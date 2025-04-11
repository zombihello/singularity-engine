/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_BOUNDSHADERSTATE_CACHE_H
#define VK_STUDIOAPI_BOUNDSHADERSTATE_CACHE_H

#include "core/types.h"

// Forward declarations
class CStudioAPIVertexDeclarationVk;
class CStudioAPIVertexShaderVk;
class CStudioAPIPixelShaderVk;
class CStudioAPIHullShaderVk;
class CStudioAPIDomainShaderVk;
class CStudioAPIGeometryShaderVk;
class CStudioAPIBoundShaderStateVk;

/**
 * @ingroup studioapi_vk
 * @breif Key used to map a set of unique decl/vs/ps combinations to a vertex shader resource
 */
class CStudioAPIBoundShaderStateKeyVk
{
public:
	/**
	 * @brief key hasher for using CStudioAPIBoundShaderStateKeyVk in std::unordered_map
	 */
	struct boundShaderStateKeyHasher_t
	{
		/**
		 * @brief Get hash from CStudioAPIBoundShaderStateKeyVk
		 * 
		 * @param key	Bound shader state key
		 * @return Return hash for the key
		 */
		FORCEINLINE std::size_t operator()( const CStudioAPIBoundShaderStateKeyVk& key ) const
		{
			return key.GetHash();
		}
	};

	/**
	 * @brief Constructor
	 *
	 * @param pVertexDeclaration	Vertex declaration
	 * @param pVertexShader			Vertex shader
	 * @param pPixelShader			Pixel shader
	 * @param pHullShader			Hull shader
	 * @param pDomainShader			Domain shader
	 * @param pGeometryShader		Geometry shader
	 */
	CStudioAPIBoundShaderStateKeyVk( CStudioAPIVertexDeclarationVk* pVertexDeclaration, CStudioAPIVertexShaderVk* pVertexShader, CStudioAPIPixelShaderVk* pPixelShader, CStudioAPIHullShaderVk* pHullShader = NULL, CStudioAPIDomainShaderVk* pDomainShader = NULL, CStudioAPIGeometryShaderVk* pGeometryShader = NULL );

	/**
	 * @brief Get hash for the key
	 * @return Return hash for the key
	 */
	FORCEINLINE uint64 GetHash() const
	{
		return hash;
	}

	/**
	 * @brief Overrload operator '<'
	 * 
	 * @param other		Other a key
	 * @return Return TRUE if the key less the second
	 */
	FORCEINLINE bool operator<( const CStudioAPIBoundShaderStateKeyVk& other ) const
	{
		return hash < other.hash;
	}

	/**
	 * @brief Overrload operator '=='
	 * 
	 * @param other		Other a key
	 * @return Return TRUE if the key equal the second
	 */
	FORCEINLINE bool operator==( const CStudioAPIBoundShaderStateKeyVk& other ) const
	{
		return hash == other.hash;
	}

public:
	uint64		hash;	/**< Hash */
};

/**
 * @ingroup studioapi_vk
 * @breif A list of the most recently used bound shader states
 */
class CStudioAPIBoundShaderStateCacheVk
{
public:
	/**
	 * @brief Add a bound shader state to the cache
	 * 
	 * @param key				Key of bound shader state
	 * @param pBoundShaderState	Bound shader state
	 */
	FORCEINLINE void Add( const CStudioAPIBoundShaderStateKeyVk& key, CStudioAPIBoundShaderStateVk* pBoundShaderState )
	{
		boundShaderStateDict[key] = pBoundShaderState;
	}

	/**
	 * @brief Find a bound shader state by a key
	 * 
	 * @param key	Key of bound shader state
	 * @return Return bound shader state. If not found return NULL
	 */
	FORCEINLINE CStudioAPIBoundShaderStateVk* Find( const CStudioAPIBoundShaderStateKeyVk& key ) const
	{
		auto		itBoundShaderState = boundShaderStateDict.find( key );
		if ( itBoundShaderState == boundShaderStateDict.end() )
		{
			return NULL;
		}
		return itBoundShaderState->second;
	}

	/**
	 * @brief Remove the bound shader state from the cache
	 * @param key	Key of bound shader state
	 */
	FORCEINLINE void Remove( const CStudioAPIBoundShaderStateKeyVk& key )
	{
		boundShaderStateDict.erase( key );
	}

	/**
	 * @brief Remove all bound shader states from the cache
	 */
	FORCEINLINE void RemoveAll()
	{
		boundShaderStateDict.clear();
	}

private:
	std::unordered_map<CStudioAPIBoundShaderStateKeyVk, CStudioAPIBoundShaderStateVk*, CStudioAPIBoundShaderStateKeyVk::boundShaderStateKeyHasher_t>		boundShaderStateDict;	/**< Bound shader state dictionary */
};

#endif // !VK_STUDIOAPI_BOUNDSHADERSTATE_CACHE_H