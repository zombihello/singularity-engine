#pragma once

/*
==================
CShaderCompilerEnvironment::CShaderCompilerEnvironment
==================
*/
FORCEINLINE CShaderCompilerEnvironment::CShaderCompilerEnvironment( studioAPIShaderType_t type )
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
FORCEINLINE CShaderCompilerEnvironment::CShaderCompilerEnvironment( const CShaderCompilerEnvironment& copy )
	: includeDirs( copy.includeDirs )
	, defines( copy.defines )
	, compileFlags( copy.compileFlags )
	, vertexFactoryFile( copy.vertexFactoryFile )
{
}

/*
==================
CShaderCompilerEnvironment::AddIncludeDir
==================
*/
FORCEINLINE void CShaderCompilerEnvironment::AddIncludeDir( const char* pPath )
{
	includeDirs.emplace_back( pPath );
}

/*
==================
CShaderCompilerEnvironment::AddDefine
==================
*/
FORCEINLINE void CShaderCompilerEnvironment::AddDefine( const char* pName, const char* pValue )
{
	defines[pName] = pValue;
}

/*
==================
CShaderCompilerEnvironment::SetCompileFlags
==================
*/
FORCEINLINE void CShaderCompilerEnvironment::SetCompileFlags( uint32 flags )
{
	compileFlags = flags;
}

/*
==================
CShaderCompilerEnvironment::SetVertexFactoryFile
==================
*/
FORCEINLINE void CShaderCompilerEnvironment::SetVertexFactoryFile( const char* pPath )
{
	vertexFactoryFile = pPath;
}
