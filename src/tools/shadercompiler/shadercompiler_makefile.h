#pragma once
#include "stdlib/jsondoc.h"
#include "tools/shadercompiler/shadercompiler.h"

//-----------------------------------------------------------------------------
// Shader compiler makefile
//-----------------------------------------------------------------------------
class CShaderCompilerMakeFile
{
public:
	bool LoadMakeFile( const achar* pPath, const achar* pBaseDir = "" );
	bool LoadShader( const achar* pPath, const achar* pBaseDir = "" );
	FORCEINLINE void AddIncludeDir( const achar* pPath, const achar* pBaseDir = "" )
	{
		std::string		absoluteIncludeDir;
		S_MakeAbsolutePath( pPath, absoluteIncludeDir, pBaseDir );
		includeDirs.emplace_back( absoluteIncludeDir );
	}
	FORCEINLINE void Clear()
	{
		includeDirs.clear();
		shaders.clear();
	}

	FORCEINLINE const std::vector<std::string>& GetIncludeDirs() const
	{
		return includeDirs;
	}
	FORCEINLINE const std::vector<shader_t>& GetShaders() const
	{
		return shaders;
	}

private:
	std::vector<std::string>	includeDirs;
	std::vector<shader_t>		shaders;
};