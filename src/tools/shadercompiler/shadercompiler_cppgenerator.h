#pragma once
#include "tools/shadercompiler/shadercompiler.h"

//-----------------------------------------------------------------------------
// Generator of C++ code for a shader
//-----------------------------------------------------------------------------
class CShaderCompilerCppGenerator
{
public:
	void Generate( const shader_t& shader );

	FORCEINLINE void Reset()							{ buffer.clear(); }
	FORCEINLINE const std::string& GetBuffer() const	{ return buffer; }

private:
	void GenerateVar( const shaderFlag_t& shaderFlag );
	void GenerateConstructor( const achar* pClassName, const shader_t& shader );
	void GenerateGetIndexFunc( const shader_t& shader );

	std::string		buffer;
};