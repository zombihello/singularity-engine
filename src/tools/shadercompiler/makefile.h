/**
 * @file
 * @addtogroup shadercompiler shadercompiler
 */

#ifndef MAKEFILE_H
#define MAKEFILE_H

#include "stdlib/jsondoc.h"
#include "tools/shadercompiler/shadercompiler.h"

/**
 * @ingroup shadercompiler
 * @brief Shader compile makefile
 */
class CMakeFile
{
public:
	/**
	 * @brief Constructor
	 */
	CMakeFile();

	/**
	 * @brief Load a makefile
	 *
	 * @param pPath		Path to the makefile
	 * @return Return TRUE if the makefile loaded, otherwise returns FALSE
	 */
	bool LoadMakeFile( const achar* pPath );

	/**
	 * @brief Clear makefile
	 */
	void Clear();

	/**
	 * @brief Set output path for generated C++ classes
	 * 
	 * @param makeFileType	Output path for specific makefile type
	 * @param pPath			New output path
	 */
	FORCEINLINE void SetCppGenOutput( const achar* pPath )
	{
		cppGenOutput = pPath;
	}

	/**
	 * @brief Get output path for generated C++ classes
	 * 
	 * @param makeFileType	Output path for specific makefile type
	 * @return Return output path for generated C++ classes. If not set returns 'generated/'
	 */
	FORCEINLINE const std::string& GetCppGenOutput() const
	{
		return cppGenOutput;
	}

	/**
	 * @brief Set output directory for compiled shader cache
	 * @param pPath		Output directory for compiled shader cache
	 */
	FORCEINLINE void SetCacheOutputDir( const achar* pPath )
	{
		cacheOutputDir = pPath;
	}

	/**
	 * @brief Get output directory for compiled shader cache
	 * @return Return output directory for compiled shader cache
	 */
	FORCEINLINE const std::string& GetCacheOutputDir() const
	{
		return cacheOutputDir;
	}

	/**
	 * @brief Get array of shader's include directories
	 * @return Return array of shader's include directories
	 */
	FORCEINLINE const std::vector<std::string>& GetShaderIncludeDirs() const
	{
		return shaderIncludeDirs;
	}

	/**
	 * @brief Get array of shaders to compilation
	 * @return Return array of shaders to compilation
	 */
	FORCEINLINE const std::vector<shader_t>& GetShaders() const
	{
		return shaders;
	}

	/**
	 * @brief Get path to directory where the makefile
	 * @return Return path to directory where the makefile
	 */
	FORCEINLINE const std::string GetMakeFileDir() const
	{
		return makeFileDir;
	}

	/**
	 * @brief Get shader list name
	 * @return Return shader list name
	 */
	FORCEINLINE const std::string& GetShaderListName() const
	{
		return shaderListName;
	}

private:
	/**
	 * @brief Load a C++ generate output from JSON value
	 *
	 * @param jsonValue		JSON value
	 * @param cppGenOutput	Loaded C++ generate output
	 * @return Return TRUE if C++ generate output loaded, otherwise returns FALSE
	 */
	bool LoadCppGenOutput( const CJsonValue& jsonValue, std::string& cppGenOutput );

	/**
	 * @brief Load output directory for compiled shader cache from JSON value
	 * 
	 * @param jsonValue			JSON value
	 * @param cacheOutputDir	Loaded output directory for compiled shader cache
	 * @return Return TRUE if output directory loaded, otherwise returns FALSE
	 */
	bool LoadCacheOutputDir( const CJsonValue& jsonValue, std::string& cacheOutputDir );

	/**
	 * @brief Load shader list name from JSON value
	 *
	 * @param jsonValue			JSON value
	 * @param shaderListName	Loaded shader list name
	 * @return Return TRUE if shader list name loaded, otherwise returns FALSE
	 */
	bool LoadShaderListName( const CJsonValue& jsonValue, std::string& shaderListName );

	/**
	 * @brief Load defines from JSON array
	 * 
	 * @param jsonArray		JSON array
	 * @param defines		Loaded defines
	 * @return Return TRUE if defines loaded, otherwise returns FALSE
	 */
	bool LoadDefines( const std::vector<CJsonValue>& jsonArray, std::vector<shaderDefine_t>& defines );

	/**
	 * @brief Load shader from JSON object
	 * 
	 * @param jsonObject	JSON object
	 * @param shader		Loaded shader
	 * @return Return TRUE if shader loaded, otherwise returns FALSE
	 */
	bool LoadShader( const CJsonObject& jsonObject, shader_t& shader );

	/**
	 * @brief Load flags from JSON array
	 *
	 * @param jsonArray		JSON array
	 * @param flags			Loaded flags
	 * @return Return TRUE if flags loaded, otherwise returns FALSE
	 */
	bool LoadFlags( const std::vector<CJsonValue>& jsonArray, std::vector<shaderFlag_t>& flags );

	std::string					shaderListName;			/**< Shader list name */
	std::string					makeFileDir;			/**< Path to directory where the makefile */
	std::string					cacheOutputDir;			/**< Output directory for compiled shader cache */
	std::string					cppGenOutput;			/**< Output path for generated C++ classes */
	std::vector<std::string>	shaderIncludeDirs;		/**< Array of shader's include directories */
	std::vector<shader_t>		shaders;				/**< Array of shaders to compilation */
};

#endif // !MAKEFILE_H