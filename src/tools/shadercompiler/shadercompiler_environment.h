#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>

#include "tools/shadercompiler/ishadercompiler_backend.h"

//-----------------------------------------------------------------------------
// The environment used to compile shader
//-----------------------------------------------------------------------------
class CShaderCompilerEnvironment : public IShaderCompilerEnvironment
{
public:
	// IShaderCompilerEnvironment interface
	virtual uint32		GetCompileFlags() const override;
	virtual uint32		GetNumIncludeDirs() const override;
	virtual uint32		GetNumDefines() const override;
	virtual const char* GetIncludeDir( uint32 index ) const override;
	virtual void		GetDefine( uint32 index, const char*& pName, const char*& pValue ) const override;

	// Absolute path to the vertex factory implementation, can be NULL/empty
	// when the shader isn't using vertex factories
	virtual const char* GetVertexFactoryFile() const override;

	CShaderCompilerEnvironment( studioAPIShaderType_t type );
	CShaderCompilerEnvironment( const CShaderCompilerEnvironment& copy );

	void AddIncludeDir( const char* pPath );
	void AddDefine( const char* pName, const char* pValue );
	void SetCompileFlags( uint32 flags );
	void SetVertexFactoryFile( const char* pPath );

private:
	uint32											   compileFlags;
	eastl::string									   vertexFactoryFile;
	eastl::vector<eastl::string>					   includeDirs;
	eastl::unordered_map<eastl::string, eastl::string> defines;
};

#include "tools/shadercompiler/shadercompiler_environment.inl"
