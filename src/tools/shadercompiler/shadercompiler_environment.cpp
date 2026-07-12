#include "pch_shadercompiler.h"
#include "tools/shadercompiler/shadercompiler_environment.h"

/*
==================
CShaderCompilerEnvironment::GetCompileFlags
==================
*/
uint32 CShaderCompilerEnvironment::GetCompileFlags() const
{
	return compileFlags;
}

/*
==================
CShaderCompilerEnvironment::GetNumIncludeDirs
==================
*/
uint32 CShaderCompilerEnvironment::GetNumIncludeDirs() const
{
	return (uint32)includeDirs.size();
}

/*
==================
CShaderCompilerEnvironment::GetNumDefines
==================
*/
uint32 CShaderCompilerEnvironment::GetNumDefines() const
{
	return (uint32)defines.size();
}

/*
==================
CShaderCompilerEnvironment::GetIncludeDir
==================
*/
const char* CShaderCompilerEnvironment::GetIncludeDir( uint32 index ) const
{
	Assert( index >= 0 && index < (uint32)includeDirs.size() );
	return includeDirs[index].c_str();
}

/*
==================
CShaderCompilerEnvironment::GetDefine
==================
*/
void CShaderCompilerEnvironment::GetDefine( uint32 index, const char*& pName, const char*& pDefine ) const
{
	Assert( index >= 0 && index < (uint32)defines.size() );

	uint32 elementID = 0;
	for ( auto it = defines.begin(), itEnd = defines.end(); it != itEnd; ++it, ++elementID )
	{
		if ( elementID == index )
		{
			pName	= it->first.c_str();
			pDefine = it->second.c_str();
			return;
		}
	}

	pName	= "";
	pDefine = "";
}

/*
==================
CShaderCompilerEnvironment::GetVertexFactoryFile
==================
*/
const char* CShaderCompilerEnvironment::GetVertexFactoryFile() const
{
	return vertexFactoryFile.c_str();
}
