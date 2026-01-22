#include "pch_shadercompiler.h"
#include "tools/shadercompiler/shadercompiler_environment.h"

/*
==================
CShaderCompilerEnvironment::CShaderCompilerEnvironment
==================
*/
CShaderCompilerEnvironment::CShaderCompilerEnvironment( studioAPIShaderType_t type )
	: compileFlags( 0x0 )
{
	defines.insert( eastl::make_pair( "VERTEX_SHADER", type == STUDIOAPI_SHADER_TYPE_VERTEX ? "1" : "0" ) );
	defines.insert( eastl::make_pair( "DOMAIN_SHADER", type == STUDIOAPI_SHADER_TYPE_DOMAIN ? "1" : "0" ) );
	defines.insert( eastl::make_pair( "HULL_SHADER", type == STUDIOAPI_SHADER_TYPE_HULL ? "1" : "0" ) );
	defines.insert( eastl::make_pair( "GEOMETRY_SHADER", type == STUDIOAPI_SHADER_TYPE_GEOMETRY ? "1" : "0" ) );
	defines.insert( eastl::make_pair( "PIXEL_SHADER", type == STUDIOAPI_SHADER_TYPE_PIXEL ? "1" : "0" ) );
	defines.insert( eastl::make_pair( "COMPUTE_SHADER", type == STUDIOAPI_SHADER_TYPE_COMPUTE ? "1" : "0" ) );
}

/*
==================
CShaderCompilerEnvironment::CShaderCompilerEnvironment
==================
*/
CShaderCompilerEnvironment::CShaderCompilerEnvironment( const CShaderCompilerEnvironment& copy )
	: includeDirs( copy.includeDirs )
	, defines( copy.defines )
	, compileFlags( copy.compileFlags )
{
}

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