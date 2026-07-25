#pragma once
#include "studiorender/istudio_renderresource.h"
#include "tier0/types.h"
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdList;
class IStudioAPIRenderPipeline;
class IMaterialVar;
class IVertexFactory;

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
	const char*		  pName;
	shaderParamType_t type;
	const char*		  pHelpString;
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
// Per-material context data interface
// NOTE: Shaders can keep per material data in classes descended from this
//-----------------------------------------------------------------------------
class IPerMaterialContextData : public IRefCounted
{
public:
	virtual ~IPerMaterialContextData() {}
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

	// Function create and update per-material context data from material vars
	// NOTE: `pParams` must be size equal to shader parameters
	virtual CRefPtr<IPerMaterialContextData> CreatePerMaterialContextData( IMaterialVar** pParams ) const													= 0;
	virtual void							 UpdatePerMaterialContextData( IMaterialVar** pParams, IPerMaterialContextData* pPerMaterialContextData ) const = 0;

	// Place barriers into a command list
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList, IPerMaterialContextData* pPerMaterialContextData ) const = 0;

	// Bind the shader's own resources (textures, samplers, constant buffers, etc) into the command list
	virtual void R_Bind( IStudioAPICmdList* pStudioAPICmdList, IPerMaterialContextData* pPerMaterialContextData ) = 0;

	// Resolve (baking if needed) the render pipeline for the current combo
	// NOTE: `pVertexFactory` can be NULL when the shader generates its own geometry
	virtual IStudioAPIRenderPipeline* R_ResolveRenderPipeline( IPerMaterialContextData* pPerMaterialContextData, IVertexFactory* pVertexFactory, studioRenderPassType_t renderPassType ) = 0;

	virtual uint32		  GetNumParams() const				 = 0;
	virtual shaderParam_t GetParam( uint32 index ) const	 = 0;
	virtual uint32		  GetNumCacheNames() const			 = 0;
	virtual const char*	  GetCacheName( uint32 index ) const = 0;
	virtual uint32		  GetFlags() const					 = 0;
	virtual const char*	  GetName() const					 = 0;
	virtual const char*	  GetHelp() const					 = 0;
	virtual const char*	  GetFallbackShader() const			 = 0;
};
