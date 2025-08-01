#pragma once
#include "stdlib/jsondoc.h"
#include "tools/shadercompiler/shadercompiler.h"

//-----------------------------------------------------------------------------
// Shader compile makefile
//-----------------------------------------------------------------------------
class CMakeFile
{
public:
	CMakeFile();

	bool LoadMakeFile( const achar* pPath );
	void Clear();

	FORCEINLINE void SetCppGenOutput( const achar* pPath )							{ cppGenOutput = pPath; }
	FORCEINLINE void SetCacheOutputDir( const achar* pPath )						{ cacheOutputDir = pPath; }
	FORCEINLINE const std::string& GetCppGenOutput() const							{ return cppGenOutput; }
	FORCEINLINE const std::string& GetCacheOutputDir() const						{ return cacheOutputDir; }
	FORCEINLINE const std::vector<std::string>& GetShaderIncludeDirs() const		{ return shaderIncludeDirs; }
	FORCEINLINE const std::vector<shader_t>& GetShaders() const						{ return shaders; }
	FORCEINLINE const std::string GetMakeFileDir() const							{ return makeFileDir; }
	FORCEINLINE const std::string& GetShaderListName() const						{ return shaderListName; }

private:
	bool LoadCppGenOutput( const CJsonValue& jsonValue, std::string& cppGenOutput );
	bool LoadCacheOutputDir( const CJsonValue& jsonValue, std::string& cacheOutputDir );
	bool LoadShaderListName( const CJsonValue& jsonValue, std::string& shaderListName );
	bool LoadDefines( const std::vector<CJsonValue>& jsonArray, std::vector<shaderDefine_t>& defines );
	bool LoadShader( const CJsonObject& jsonObject, shader_t& shader );
	bool LoadFlags( const std::vector<CJsonValue>& jsonArray, std::vector<shaderFlag_t>& flags );

	std::string					shaderListName;
	std::string					makeFileDir;
	std::string					cacheOutputDir;
	std::string					cppGenOutput;
	std::vector<std::string>	shaderIncludeDirs;
	std::vector<shader_t>		shaders;
};