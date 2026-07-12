#pragma once
#include "tier1/stl.h"
#include "tools/shadercompiler/shadercompiler.h"

//-----------------------------------------------------------------------------
// Shader compiler makefile
//-----------------------------------------------------------------------------
class CShaderCompilerMakeFile
{
public:
	bool			 LoadMakeFile( const char* pPath, const char* pBaseDir = "" );
	bool			 LoadShader( const char* pPath, const char* pBaseDir = "" );
	bool			 LoadVertexFactoryList( const char* pPath, const char* pBaseDir = "" );
	FORCEINLINE void AddIncludeDir( const char* pPath, const char* pBaseDir = "" )
	{
		eastl::string absoluteIncludeDir;
		S_MakeAbsolutePath( pPath, absoluteIncludeDir, pBaseDir );
		includeDirs.emplace_back( absoluteIncludeDir );
	}
	FORCEINLINE void Clear()
	{
		includeDirs.clear();
		shaders.clear();
		vertexFactories.clear();
		vertexFactoryDict.clear();
	}

	FORCEINLINE const eastl::vector<eastl::string>& GetIncludeDirs() const
	{
		return includeDirs;
	}
	FORCEINLINE const eastl::vector<shader_t>& GetShaders() const
	{
		return shaders;
	}
	FORCEINLINE const eastl::vector<vertexFactory_t>& GetVertexFactories() const
	{
		return vertexFactories;
	}

private:
	eastl::vector<eastl::string>																		   includeDirs;
	eastl::vector<shader_t>																				   shaders;
	eastl::vector<vertexFactory_t>																		   vertexFactories;
	eastl::unordered_map<eastl::string, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> vertexFactoryDict;
};
