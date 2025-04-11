/**
 * @file
 * @addtogroup shadercache shadercache
 */

#ifndef SHADERCACHE_H
#define SHADERCACHE_H

#include <vector>

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/istreamdata.h"
#include "studiorender/studioapi/istudioapi_shader.h"

/**
 * @ingroup shadercache
 * @brief Helper for work with shader cache files
 */
class CShaderCacheDoc
{
public:
	/**
	 * @brief Shader cache
	 */
	struct shaderCache_t
	{
		/**
		 * @brief Constructor
		 */
		shaderCache_t()
		{}

		/**
		 * @brief Constructor
		 * @param entryPointName	Entry point name
		 * @param reflectionData	Reflection data
		 * @param bytecode			Bytecode
		 */
		shaderCache_t( const std::string& entryPointName, const std::vector<byte>& reflectionData, const std::vector<byte>& bytecode )
			: entryPointName( entryPointName )
			, reflectionData( reflectionData )
			, bytecode( bytecode )
		{}

		/**
		 * @brief Serialize the data into a stream
		 * @param pStreamWriter		Stream data writer
		 */
		void Serialize( IStreamDataWriter* pStreamWriter );

		/**
		 * @brief Deserialize a data from a stream
		 * @param pStreamReader		Stream data reader
		 */
		void Deserialize( IStreamDataReader* pStreamReader );

		std::string				entryPointName;		/**< Entry point name */
		std::vector<byte>		reflectionData;		/**< Reflection data */
		std::vector<byte>		bytecode;			/**< Bytecode */
	};

	/**
	 * @brief Constructor
	 */
	CShaderCacheDoc();

	/**
	 * @brief Set shader type
	 * @param type	Shader type
	 */
	FORCEINLINE void SetType( studioAPIShaderType_t type )
	{
		CShaderCacheDoc::type = type;
	}

	/**
	 * @brief Get shader type
	 * @return Return shader type
	 */
	FORCEINLINE studioAPIShaderType_t GetType() const
	{
		return type;
	}

	/**
	 * @brief Add cache
	 * @param entryPointName	Entry point name
	 * @param reflectionData	Reflection data
	 * @param bytecode			Bytecode
	 */
	FORCEINLINE void AddCache( const std::string& entryPointName, const std::vector<byte>& reflectionData, const std::vector<byte>& bytecode )
	{
		caches.emplace_back( entryPointName, reflectionData, bytecode );
	}

	/**
	 * @brief Get number of shader caches
	 * @return Return number of caches for specific vertex factory type
	 */
	FORCEINLINE uint64 GetNumCaches() const
	{
		return ( uint64 )caches.size();
	}

	/**
	 * @brief Get shader cache by index
	 * @param cacheId		Shader cache index
	 * @return Return shader cache at index
	 */
	FORCEINLINE const shaderCache_t& GetCache( uint64 cacheId ) const
	{
		Assert( cacheId < ( uint64 )caches.size() );
		return caches[cacheId];
	}

	/**
	 * @brief Save shader cache document
	 * @warning For use StdLib must be connected by ConnectStdLib
	 *
	 * @param pPath		Path to save
	 * @return Return TRUE if shader cache was successes saved, otherwise return FALSE
	 */
	bool SaveFile( const achar* pPath );

	/**
	 * @brief Load from file
	 * @warning For use StdLib must be connected by ConnectStdLib
	 *
	 * @param pPath		Path to file
	 * @return Return TRUE when file success loaded, otherwise return FALSE
	 */
	bool LoadFromFile( const achar* pPath );

	/**
	 * @brief Clear shader cache
	 */
	void Clear();

private:
	studioAPIShaderType_t		type;		/**< Shader type */
	std::vector<shaderCache_t>	caches;		/**< Shader caches */
};

#endif // !SHADERCACHE_H