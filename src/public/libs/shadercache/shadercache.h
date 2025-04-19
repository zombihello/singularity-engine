#ifndef SHADERCACHE_H
#define SHADERCACHE_H

#include <vector>

#include "core/types.h"
#include "core/platform.h"
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
		{}

		shaderCache_t( const std::string& entryPointName, const std::vector<byte>& reflectionData, const std::vector<byte>& bytecode )
			: entryPointName( entryPointName )
			, reflectionData( reflectionData )
			, bytecode( bytecode )
		{}

		void Serialize( IStreamDataWriter* pStreamWriter );
		void Deserialize( IStreamDataReader* pStreamReader );

		std::string				entryPointName;
		std::vector<byte>		reflectionData;
		std::vector<byte>		bytecode;
	};

	CShaderCacheDoc();

	// Save, load and clear a shader cache document
	// NOTE: For use save and load functions StdLib must be connected
	bool SaveFile( const achar* pPath );
	bool LoadFromFile( const achar* pPath );
	void Clear();

	// Set/get type
	void SetType( studioAPIShaderType_t type );
	studioAPIShaderType_t GetType() const;

	// Add and get a shader cache
	void AddCache( const std::string& entryPointName, const std::vector<byte>& reflectionData, const std::vector<byte>& bytecode );
	uint64 GetNumCaches() const;
	const shaderCache_t& GetCache( uint64 cacheId ) const;

private:
	studioAPIShaderType_t		type;
	std::vector<shaderCache_t>	caches;
};

#include "shadercache/shadercache.inl"

#endif // !SHADERCACHE_H