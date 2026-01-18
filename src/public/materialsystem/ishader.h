#pragma once
#include "stdlib/types.h"
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdList;
class IMaterialVar;

//-----------------------------------------------------------------------------
// Shader flags and parameter types
//-----------------------------------------------------------------------------
enum shaderFlags_t
{
	SHADER_NOT_EDITABLE = 1 << 1
};

enum shaderParamFlags_t
{
	SHADER_PARAM_NOT_EDITABLE = 1 << 1
};

enum shaderParamType_t
{
	SHADER_PARAM_TYPE_UNDEFINED,
	SHADER_PARAM_TYPE_BOOL,
	SHADER_PARAM_TYPE_INT,
	SHADER_PARAM_TYPE_FLOAT,
	SHADER_PARAM_TYPE_VECTOR_2D,
	SHADER_PARAM_TYPE_VECTOR_3D,
	SHADER_PARAM_TYPE_VECTOR_4D,
	SHADER_PARAM_TYPE_MATRIX,
	SHADER_PARAM_TYPE_STRING,
	SHADER_PARAM_TYPE_TEXTURE,
	SHADER_PARAM_TYPE_MATERIAL
};

//-----------------------------------------------------------------------------
// Shader parameter and cache information
//-----------------------------------------------------------------------------
struct shaderParam_t
{
	const char*	  pName;
	shaderParamType_t type;
	const char*	  pHelpString;
	uint32			  flags;  // see shaderParamFlags_t
};

struct shaderCacheInfo_t
{
	uint64 numCaches;
	uint64 indexOffset;	 // Cache index offset in the shader manager
};

struct shaderInitParams_t
{
	shaderCacheInfo_t cacheInfos[STUDIOAPI_SHADER_NUM_DRAW_TYPES];
};

//-----------------------------------------------------------------------------
// Shader interface
//-----------------------------------------------------------------------------
class IShader
{
public:
	// Initialize and shutdown
	virtual void Init( const shaderInitParams_t& shaderInitParams ) = 0;
	virtual void InitDefaultParams( IMaterialVar** pParams ) const	= 0;
	virtual void Shutdown()											= 0;

	// NOTE: pStudioAPIBuffers and pParams Must be size equal to shader parameters and buffers count
	virtual void R_UpdateBuffers( IStudioAPICmdContext* pStudioAPICmdContext, TRefPtr<IStudioAPIBuffer>* pStudioAPIBuffers, IMaterialVar** pParams ) const							  = 0;
	virtual void R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers = NULL ) = 0;

	// Place barriers into a command list
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers ) const = 0;

	virtual uint32		  GetNumParams() const				 = 0;
	virtual shaderParam_t GetParam( uint32 index ) const	 = 0;
	virtual uint32		  GetNumCacheNames() const			 = 0;
	virtual const char*  GetCacheName( uint32 index ) const = 0;
	virtual uint32		  GetNumBuffers() const				 = 0;
	virtual uint32		  GetFlags() const					 = 0;
	virtual const char*  GetName() const					 = 0;
	virtual const char*  GetHelp() const					 = 0;
	virtual const char*  GetFallbackShader() const			 = 0;
};