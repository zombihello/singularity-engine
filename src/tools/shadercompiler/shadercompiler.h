#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier0/types.h"
#include "tier0/memory.h"

//-----------------------------------------------------------------------------
// Shader compiler modes
//-----------------------------------------------------------------------------
enum shaderCompilerMode_t
{
	SHADER_COMPILER_MODE_COMPILE,
	SHADER_COMPILER_MODE_GENCPP,
	SHADER_COMPILER_MODE_NUM
};

//-----------------------------------------------------------------------------
// Shader system flag
// A system flag is a combination flag that the compiler itself
// defines and manages the meaning of
//-----------------------------------------------------------------------------
enum shaderSystemFlag_t
{
	SHADER_SYSTEM_FLAG_VERTEXFACTORY,
	SHADER_SYSTEM_FLAG_NUM
};

//-----------------------------------------------------------------------------
// Reserved vertex factory key word
//-----------------------------------------------------------------------------
enum vertexFactoryKeyWord_t
{
	VERTEXFACTORY_KEYWORD_ALL,	 // Entry means it uses every vertex factory known to the loaded list(s)
	VERTEXFACTORY_KEYWORD_NONE,	 // Entry means it can also be drawn without any vertex factory (e.g. a shader that generates its geometry from SV_VertexID)
	VERTEXFACTORY_NUM_KEYWORDS
};

//-----------------------------------------------------------------------------
// Shader compiler structs
//-----------------------------------------------------------------------------
struct shaderFlag_t
{
	eastl::string name;
	int32		  minValue;
	int32		  maxValue;
	bool		  bSetDefault;
	int32		  defaultValue;
};

struct shaderDefine_t
{
	eastl::string name;
	eastl::string value;
};

struct vertexFactory_t
{
	eastl::string name;
	eastl::string source;
	uint32		  index;  // Global vertex factory index (matches modelVertexType_t)

	static const vertexFactory_t s_none;
};

struct shader_t
{
	shader_t();

	void  AddFlag( const shaderFlag_t& flag );
	void  AddSystemFlag( const shaderFlag_t& flag, shaderSystemFlag_t type );
	void  CalcNumFlagCombos();
	int32 GetSystemFlagValue( shaderSystemFlag_t systemFlag, const int32* pFlagValues ) const;

	uint32								  numFlagCombos;
	studioAPIShaderType_t				  type;
	eastl::string						  source;
	eastl::string						  mainFunc;
	eastl::vector<shaderFlag_t>			  flags;
	eastl::vector<shaderDefine_t>		  defines;
	eastl::vector<const vertexFactory_t*> vertexFactories;							  // Empty if the shader doesn't use vertex factories
	uint32								  systemFlagIndices[SHADER_SYSTEM_FLAG_NUM];  // Where each system flag (if used) lives within `flags`, INVALID_INDEX if the flag isn't
};

//-----------------------------------------------------------------------------
// General functions
//-----------------------------------------------------------------------------
bool		ConvStringToShaderType( const char* pShaderTypeName, studioAPIShaderType_t& shaderType );
const char* ConvShaderTypeToString( studioAPIShaderType_t shaderType );
bool		ConvStringToShaderCompilerMode( const char* pShaderCompilerModeName, shaderCompilerMode_t& shaderCompilerMode );
const char* ConvShaderCompilerModeToString( shaderCompilerMode_t shaderCompilerMode );
const char* ConvShaderSystemFlagToString( shaderSystemFlag_t systemFlag );
bool		ConvStringToVertexFactoryKeyWord( const char* pKeyWordName, vertexFactoryKeyWord_t& keyWord );
const char* ConvVertexFactoryKeyWordToString( vertexFactoryKeyWord_t keyWord );

#include "tools/shadercompiler/shadercompiler.inl"
