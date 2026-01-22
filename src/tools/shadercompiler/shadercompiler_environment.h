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

	CShaderCompilerEnvironment( studioAPIShaderType_t type );
	CShaderCompilerEnvironment( const CShaderCompilerEnvironment& copy );

	FORCEINLINE void AddIncludeDir( const char* pPath ) { includeDirs.push_back( pPath ); }
	FORCEINLINE void AddDefine( const char* pName, const char* pValue ) { defines[pName] = pValue; }
	FORCEINLINE void SetCompileFlags( uint32 flags ) { compileFlags = flags; }

private:
	eastl::vector<eastl::string>					   includeDirs;
	eastl::unordered_map<eastl::string, eastl::string> defines;
	uint32											   compileFlags;
};
