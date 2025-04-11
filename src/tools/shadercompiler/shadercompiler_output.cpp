#include "pch_shadercompiler.h"
#include "tools/shadercompiler/shadercompiler_output.h"

/*
==================
CShaderCompilerOutput::CShaderCompilerOutput
==================
*/
CShaderCompilerOutput::CShaderCompilerOutput()
{}

/*
==================
CShaderCompilerOutput::SetBytecode
==================
*/
void CShaderCompilerOutput::SetBytecode( const byte* pData, uint64 size )
{
	bytecode.resize( size );
	if ( size > 0 )
	{
		Assert( pData );
		Mem_Memcpy( bytecode.data(), pData, size );
	}
}

/*
==================
CShaderCompilerOutput::SetErrorMsg
==================
*/
void CShaderCompilerOutput::SetErrorMsg( const achar* pMsg )
{
	errorMsg = pMsg;
}

/*
==================
CShaderCompilerOutput::SetReflectionData
==================
*/
void CShaderCompilerOutput::SetReflectionData( const byte* pData, uint64 size )
{
	reflectionData.resize( size );
	if ( size > 0 )
	{
		Assert( pData );
		Mem_Memcpy( reflectionData.data(), pData, size );
	}
}