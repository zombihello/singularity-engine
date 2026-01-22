#pragma once
#include <EASTL/vector.h>

#include "stdlib/defines.h"
#include "stdlib/types.h"
#include "stdlib/istreamdata.h"
#include "studiorender/studioapi/istudioapi_shader.h"

//-----------------------------------------------------------------------------
// Helper for work with shader cache files
//-----------------------------------------------------------------------------
class CShaderCacheDoc
{
public:
	struct shaderCache_t
	{
		shaderCache_t()
		{
		}

		shaderCache_t( const eastl::string& entryPointName, const eastl::vector<byte>& reflectionData, const eastl::vector<byte>& bytecode )
			: entryPointName( entryPointName )
			, reflectionData( reflectionData )
			, bytecode( bytecode )
		{
		}

		void Serialize( IStreamDataWriter* pStreamWriter );
		void Deserialize( IStreamDataReader* pStreamReader );

		eastl::string		entryPointName;
		eastl::vector<byte> reflectionData;
		eastl::vector<byte> bytecode;
	};

	CShaderCacheDoc();

	// Save, load and clear a shader cache document
	// NOTE: For use save and load functions StdLib must be connected
	bool SaveFile( const char* pPath );
	bool LoadFromFile( const char* pPath );
	void Clear();

	// Set/get type
	void				  SetType( studioAPIShaderType_t type );
	studioAPIShaderType_t GetType() const;

	// Add and get a shader cache
	void				 AddCache( const eastl::string& entryPointName, const eastl::vector<byte>& reflectionData, const eastl::vector<byte>& bytecode );
	uint64				 GetNumCaches() const;
	const shaderCache_t& GetCache( uint64 cacheId ) const;

private:
	studioAPIShaderType_t		 type;
	eastl::vector<shaderCache_t> caches;
};

#include "shadercache/shadercache.inl"
