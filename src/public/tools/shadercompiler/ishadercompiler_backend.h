#pragma once
#include "stdlib/types.h"
#include "stdlib/interface.h"
#include "studiorender/studioapi/istudioapi_shader.h"

//-----------------------------------------------------------------------------
// Shader compiler flags
//-----------------------------------------------------------------------------
enum shaderCompilerFlags_t
{
	SHADERCOMPILER_NONE	 = 0,
	SHADERCOMPILER_DEBUG = 1 << 0,	// Debug shader version
};

//-----------------------------------------------------------------------------
// Interface of the environment used to compile shader
//-----------------------------------------------------------------------------
class IShaderCompilerEnvironment
{
public:
	virtual uint32		 GetCompileFlags() const													= 0;
	virtual uint32		 GetNumIncludeDirs() const													= 0;
	virtual uint32		 GetNumDefines() const														= 0;
	virtual const char* GetIncludeDir( uint32 index ) const										= 0;
	virtual void		 GetDefine( uint32 index, const char*& pName, const char*& pValue ) const = 0;
};

//-----------------------------------------------------------------------------
// Interface of the output shader compile
//-----------------------------------------------------------------------------
class IShaderCompilerOutput
{
public:
	virtual void SetBytecode( const byte* pData, uint64 size )		 = 0;
	virtual void SetErrorMsg( const char* pMsg )					 = 0;
	virtual void SetReflectionData( const byte* pData, uint64 size ) = 0;
};

//-----------------------------------------------------------------------------
// Interface of shader compiler backend
//-----------------------------------------------------------------------------
#define SHADERCOMPILERBACKEND_INTERFACE_VERSION "SShaderCompilerBackend001"
class IShaderCompilerBackend
{
public:
	virtual bool Connect( createInterfaceFn_t pFactory ) = 0;
	virtual void Disconnect()							 = 0;

	virtual bool		 CompileShader( const char* pSrcFileName, const char* pFunctionName, studioAPIShaderType_t type, IShaderCompilerEnvironment* pEnvironment, IShaderCompilerOutput* pOutput ) = 0;
	virtual const char* GetShaderPlatform() const																																					  = 0;
};