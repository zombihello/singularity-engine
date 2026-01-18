#pragma once
#include <string>
#include <vector>

#include "stdlib/types.h"
#include "core/memory.h"

//-----------------------------------------------------------------------------
// Shader compiler structs
//-----------------------------------------------------------------------------
enum shaderCompilerMode_t
{
	SHADER_COMPILER_MODE_COMPILE,
	SHADER_COMPILER_MODE_GENCPP,
	SHADER_COMPILER_MODE_NUM
};

struct shaderFlag_t
{
	shaderFlag_t()
		: minValue( 0 )
		, maxValue( 0 )
		, bSetDefault( false )
		, defaultValue( 0 )
	{
	}

	std::string name;
	int32		minValue;
	int32		maxValue;
	bool		bSetDefault;
	int32		defaultValue;
};

struct shaderDefine_t
{
	std::string name;
	std::string value;
};

struct shader_t
{
	void CalcNumFlagCombos();

	uint32						numFlagCombos;
	studioAPIShaderType_t		type;
	std::string					source;
	std::string					mainFunc;
	std::vector<shaderFlag_t>	flags;
	std::vector<shaderDefine_t> defines;
};

//-----------------------------------------------------------------------------
// General functions
//-----------------------------------------------------------------------------
bool ConvStringToShaderType( const char* pShaderTypeName, studioAPIShaderType_t& shaderType );
void ConvShaderTypeToString( studioAPIShaderType_t shaderType, const char*& pShaderTypeName );
bool ConvStringToShaderCompilerMode( const char* pShaderCompilerModeName, shaderCompilerMode_t& shaderCompilerMode );
void ConvShaderCompilerModeToString( shaderCompilerMode_t shaderCompilerMode, const char*& pShaderCompilerModeName );