#ifndef SHADERCOMPILER_OUTPUT_H
#define SHADERCOMPILER_OUTPUT_H

#include <string>
#include <vector>
#include <unordered_map>

#include "tools/shadercompiler/ishadercompiler_backend.h"
#include "shadercache/shadercache.h"

//-----------------------------------------------------------------------------
// The output of the shader compile
//-----------------------------------------------------------------------------
class CShaderCompilerOutput : public IShaderCompilerOutput
{
public:
	// IShaderCompilerOutput interface
	virtual void SetBytecode( const byte* pData, uint64 size ) override;
	virtual void SetErrorMsg( const achar* pMsg ) override;
	virtual void SetReflectionData( const byte* pData, uint64 size ) override;

	CShaderCompilerOutput();

	FORCEINLINE const std::vector<byte>& GetBytecode() const			{ return bytecode; }
	FORCEINLINE const std::string& GetErrorMsg() const					{ return errorMsg; }
	FORCEINLINE const std::vector<byte>& GetReflectionData() const		{ return reflectionData; }

private:
	std::vector<byte>		bytecode;
	std::string				errorMsg;
	std::vector<byte>		reflectionData;
};

#endif // !SHADERCOMPILER_OUTPUT_H