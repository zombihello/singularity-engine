#pragma once
#include "tier0/types.h"
#include "tier1/debugname.h"
#include "studiorender/istudio_renderresource.h"
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdList;
class IStudioAPIRenderPipeline;
class IMaterialVar;
class IVertexFactory;
class ITextureResource;
class IMaterialResource;

//-----------------------------------------------------------------------------
// Shader flags, parameter types and update frequency
//-----------------------------------------------------------------------------
enum shaderFlags_t
{
	SHADER_NOT_EDITABLE = BIT( 1 )
};

enum shaderParamFlags_t
{
	SHADER_PARAM_NOT_EDITABLE = BIT( 1 )
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

enum shaderParamFrequency_t
{
	SHADER_PARAM_FREQUENCY_PERMATERIAL,	 // Params are authored on the main thread by materials and baked once into a `IPerMaterialContextData`
	SHADER_PARAM_FREQUENCY_PERDRAW,		 // Params are supplied by the renderer on the render thread as plain POD
	SHADER_PARAM_NUM_FREQUENCIES
};

//-----------------------------------------------------------------------------
// Shader parameter and cache information
//-----------------------------------------------------------------------------
struct shaderParam_t
{
	const char*			   pName;
	const char*			   pHelpString;
	shaderParamType_t	   type;
	shaderParamFrequency_t frequency;
	uint32				   flags;  // see shaderParamFlags_t
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
// Per-draw shader parameter value
//-----------------------------------------------------------------------------
struct shaderPerDrawVar_t
{
	void SetUndefined();
	void SetBoolValue( bool bValue );
	void SetIntValue( int32 value );
	void SetFloatValue( float value );
	void SetVecValue( const float* pValue, uint32 numComps );
	void SetVecValue( const vector2_t& value );
	void SetVecValue( const vector3_t& value );
	void SetVecValue( const vector4_t& value );
	void SetMatrixValue( const matrix4x4_t& value );
	void SetStringValue( const char* pValue );
	void SetTextureValue( ITextureResource* pValue );
	void SetMaterialValue( IMaterialResource* pValue );

	bool			   GetBoolValue() const;
	int32			   GetIntValue() const;
	float			   GetFloatValue() const;
	void			   GetVecValue( float* pValue, uint32 numComps ) const;
	matrix4x4_t		   GetMatrixValue() const;
	const char*		   GetStringValue() const;
	ITextureResource*  GetTextureValue() const;
	IMaterialResource* GetMaterialValue() const;

	shaderParamType_t type;
	union
	{
		bool			   boolValue;
		int32			   intValue;
		float			   floatValue;
		vector2_t		   vector2DValue;
		vector3_t		   vector3DValue;
		vector4_t		   vector4DValue;
		matrix4x4_t		   matrixValue;
		const char*		   pStringValue;
		ITextureResource*  pTextureValue;
		IMaterialResource* pMaterialValue;
	};
};

//-----------------------------------------------------------------------------
// Per-material context data interface
// NOTE: Shaders can keep per material data in classes descended from this
//-----------------------------------------------------------------------------
class IPerMaterialContextData : public IRefCounted, public IDebugNamed
{
public:
	virtual ~IPerMaterialContextData() {}

	// Update the per-material context data from material vars
	// NOTE: `pParams` must be size equal to shader parameters
	virtual void Update( IMaterialVar** pParams ) = 0;

	// Place barriers into the command list
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const = 0;
};

//-----------------------------------------------------------------------------
// Parameters supplied by the renderer for a single draw
//-----------------------------------------------------------------------------
struct shaderDrawParams_t
{
	const IPerMaterialContextData* pPerMaterialContextData;
	const shaderPerDrawVar_t*	   pPerDrawVars;
	const IVertexFactory*		   pVertexFactory;	// NOTE: Can be NULL when a shader generates its own geometry
};

//-----------------------------------------------------------------------------
// Shader interface
//-----------------------------------------------------------------------------
class IShader
{
public:
	// Initialize and shutdown
	virtual void Init( const shaderInitParams_t& shaderInitParams )		= 0;
	virtual void InitDefaultParams( IMaterialVar** pParams ) const		= 0;
	virtual void InitDefaultParams( shaderPerDrawVar_t* pParams ) const = 0;
	virtual void Shutdown()												= 0;

	// Create a per-material context data
	virtual CRefPtr<IPerMaterialContextData> CreatePerMaterialContextData( const char* pDebugName = "" ) const = 0;

	// Bind the shader's own resources (textures, samplers, constant buffers, etc) into the command list
	virtual void R_Bind( IStudioAPICmdList* pStudioAPICmdList, const shaderDrawParams_t& drawParams ) = 0;

	// Resolve (baking if needed) the render pipeline for the current combo
	virtual IStudioAPIRenderPipeline* R_ResolveRenderPipeline( const shaderDrawParams_t& drawParams, studioRenderPassType_t renderPassType ) = 0;

	virtual uint32					  FindParamIndex( shaderParamFrequency_t frequency, const char* pName ) const = 0;
	virtual uint32					  GetNumParams( shaderParamFrequency_t frequency ) const					  = 0;
	virtual shaderParam_t			  GetParam( shaderParamFrequency_t frequency, uint32 index ) const			  = 0;
	virtual const shaderPerDrawVar_t* GetDefaultPerDrawVars() const												  = 0;
	virtual uint32					  GetNumCacheNames() const													  = 0;
	virtual const char*				  GetCacheName( uint32 index ) const										  = 0;
	virtual uint32					  GetFlags() const															  = 0;
	virtual const char*				  GetName() const															  = 0;
	virtual const char*				  GetHelp() const															  = 0;
	virtual const char*				  GetFallbackShader() const													  = 0;	// Returns NULL when no have a fallback shader
};

#include "materialsystem/ishader.inl"
