#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>

#include "tools/shadercompiler/ishadercompiler_backend.h"
#include "utils/shadercache/shadercache.h"

//-----------------------------------------------------------------------------
// The output of the shader compile
//-----------------------------------------------------------------------------
class CShaderCompilerOutput : public IShaderCompilerOutput
{
public:
	// IShaderCompilerOutput interface
	virtual void SetBytecode( const byte* pData, uint64 size ) override;
	virtual void SetErrorMsg( const char* pMsg ) override;
	virtual void SetReflectionData( const byte* pData, uint64 size ) override;

	CShaderCompilerOutput();

	FORCEINLINE const eastl::vector<byte>& GetBytecode() const { return bytecode; }
	FORCEINLINE const eastl::string& GetErrorMsg() const { return errorMsg; }
	FORCEINLINE const eastl::vector<byte>& GetReflectionData() const { return reflectionData; }

private:
	eastl::vector<byte> bytecode;
	eastl::string		errorMsg;
	eastl::vector<byte> reflectionData;
};
