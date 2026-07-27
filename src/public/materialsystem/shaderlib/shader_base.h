#pragma once
#include "tier1/stl.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/istudio_renderpipelineset.h"
#include "studiorender/istudio_renderresource.h"
#include "studiorender/studio_resourcebindingslots.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/ishader.h"
#include "modelsystem/ivertexfactory.h"
#include "resourcesystem/iresourcesystem.h"

//-----------------------------------------------------------------------------
// Helper macros to begin/end implement a shader
//-----------------------------------------------------------------------------
#define BEGIN_SHADER_FLAGS( Name, HelpString, Flags )                                                                                                   \
	namespace Name                                                                                                                                      \
	{                                                                                                                                                   \
		static const char*					s_pHelpString = HelpString;                                                                                 \
		static const char*					s_pName		  = #Name;                                                                                      \
		static uint32						s_Flags		  = Flags;                                                                                      \
		static uint32						s_NumBuffers  = 0;                                                                                          \
		static eastl::vector<shaderParam_t> s_ShaderParams[SHADER_PARAM_NUM_FREQUENCIES];                                                               \
		class CShaderParam                                                                                                                              \
		{                                                                                                                                               \
		public:                                                                                                                                         \
			CShaderParam( const char* pName, shaderParamType_t type, shaderParamFrequency_t frequency, const char* pHelpString, uint32 flags )          \
				: frequency( frequency )                                                                                                                \
				, index( INVALID_INDEX )                                                                                                                \
			{                                                                                                                                           \
				index				= (uint32)s_ShaderParams[(uint32)frequency].size();                                                                 \
				shaderParam_t& info = s_ShaderParams[(uint32)frequency].emplace_back();                                                                 \
				info.pName			= pName;                                                                                                            \
				info.pHelpString	= pHelpString;                                                                                                      \
				info.type			= type;                                                                                                             \
				info.frequency		= frequency;                                                                                                        \
				info.flags			= flags;                                                                                                            \
			}                                                                                                                                           \
			operator uint32()                                                                                                                           \
			{                                                                                                                                           \
				return index;                                                                                                                           \
			}                                                                                                                                           \
			FORCEINLINE const shaderParam_t& GetInfo() const                                                                                            \
			{                                                                                                                                           \
				return s_ShaderParams[(uint32)frequency][index];                                                                                        \
			}                                                                                                                                           \
                                                                                                                                                        \
		private:                                                                                                                                        \
			shaderParamFrequency_t frequency;                                                                                                           \
			uint32				   index;                                                                                                               \
		};                                                                                                                                              \
		class CShaderBufferInfo                                                                                                                         \
		{                                                                                                                                               \
		public:                                                                                                                                         \
			CShaderBufferInfo( uint32 bindSet, uint32 bindSlot, uint32 size, uint32 usageFlags )                                                        \
				: size( size )                                                                                                                          \
				, usageFlags( usageFlags )                                                                                                              \
				, bindSet( bindSet )                                                                                                                    \
				, bindSlot( bindSlot )                                                                                                                  \
				, index( s_NumBuffers++ )                                                                                                               \
			{                                                                                                                                           \
			}                                                                                                                                           \
			operator uint32()                                                                                                                           \
			{                                                                                                                                           \
				return index;                                                                                                                           \
			}                                                                                                                                           \
			FORCEINLINE uint32 GetSize() const                                                                                                          \
			{                                                                                                                                           \
				return size;                                                                                                                            \
			}                                                                                                                                           \
			FORCEINLINE uint32 GetIndex() const                                                                                                         \
			{                                                                                                                                           \
				return index;                                                                                                                           \
			}                                                                                                                                           \
			FORCEINLINE CRefPtr<IStudioAPIBuffer> CreateBuffer( byte* pData ) const                                                                     \
			{                                                                                                                                           \
				return g_pStudioAPI->CreateBuffer( pData, size, size, usageFlags );                                                                     \
			}                                                                                                                                           \
			FORCEINLINE void UpdateBuffer( IStudioAPICmdContext* pStudioAPICmdContext, byte* pData, CRefPtr<IStudioAPIBuffer>& pStudioAPIBuffer ) const \
			{                                                                                                                                           \
				pStudioAPIBuffer->UpdateData( pStudioAPICmdContext, pData, size );                                                                      \
			}                                                                                                                                           \
			FORCEINLINE void UpdateBuffer( byte* pData, CRefPtr<IStudioAPIBuffer>& pStudioAPIBuffer ) const                                             \
			{                                                                                                                                           \
				/** Each MapMemory on a volatile buffer grabs a fresh region from the frame temp allocator */                                           \
				studioAPIMappedBufferData_t mappedData = {};                                                                                            \
				pStudioAPIBuffer->MapMemory( size, 0, mappedData );                                                                                     \
				Mem_Memcpy( mappedData.pData, pData, size );                                                                                            \
				pStudioAPIBuffer->UnmapMemory( mappedData );                                                                                            \
			}                                                                                                                                           \
			FORCEINLINE void SetConstantBuffer( IStudioAPICmdList* pStudioAPICmdList, IStudioAPIBuffer* pStudioAPIBuffer ) const                        \
			{                                                                                                                                           \
				pStudioAPICmdList->SetConstantBuffer( bindSet, bindSlot, pStudioAPIBuffer );                                                            \
			}                                                                                                                                           \
                                                                                                                                                        \
		private:                                                                                                                                        \
			uint32 size;                                                                                                                                \
			uint32 usageFlags; /* see studioAPIBufferUsageFlag_t */                                                                                     \
			uint32 bindSet;                                                                                                                             \
			uint32 bindSlot;                                                                                                                            \
			uint32 index;                                                                                                                               \
		};                                                                                                                                              \
		class CShaderTextureSamplerInfo                                                                                                                 \
		{                                                                                                                                               \
		public:                                                                                                                                         \
			CShaderTextureSamplerInfo( uint32 bindSet, uint32 bindSlot )                                                                                \
				: bindSet( bindSet )                                                                                                                    \
				, bindSlot( bindSlot )                                                                                                                  \
			{                                                                                                                                           \
			}                                                                                                                                           \
			FORCEINLINE void SetTexture( IStudioAPICmdList* pStudioAPICmdList, IStudioAPITexture* pStudioAPITexture ) const                             \
			{                                                                                                                                           \
				pStudioAPICmdList->SetTexture( bindSet, bindSlot, pStudioAPITexture );                                                                  \
			}                                                                                                                                           \
			FORCEINLINE void SetSampler( IStudioAPICmdList* pStudioAPICmdList, IStudioAPISampler* pStudioAPISampler ) const                             \
			{                                                                                                                                           \
				pStudioAPICmdList->SetSampler( bindSet, bindSlot, pStudioAPISampler );                                                                  \
			}                                                                                                                                           \
                                                                                                                                                        \
		private:                                                                                                                                        \
			uint32 bindSet;                                                                                                                             \
			uint32 bindSlot;                                                                                                                            \
		};

#define BEGIN_SHADER( Name, HelpString ) BEGIN_SHADER_FLAGS( Name, HelpString, 0 )
#define END_SHADER                   \
	}                                \
	;                                \
	static CShader s_ShaderInstance; \
	}  // Namespace

//-----------------------------------------------------------------------------
// Helper macros to declare buffer data type
//-----------------------------------------------------------------------------
#define SHADER_BUFFER_DATA_TYPE( Name ) shaderBuffer_##Name##_t
#define BEGIN_SHADER_BUFFER_DATA( Name )                   \
	ALIGN16_PREDECL struct SHADER_BUFFER_DATA_TYPE( Name ) \
	{
#define END_SHADER_BUFFER_DATA \
	}                          \
	ALIGN16_POSTDECL;
#define DECLARE_SHADER_BUFFER_DATA( Name ) SHADER_BUFFER_DATA_TYPE( Name ) Name = {};

//-----------------------------------------------------------------------------
// Helper macros to declare shader parameters
//-----------------------------------------------------------------------------
#define SHADER_PARAM_FLAGS( Name, Type, Frequency, HelpString, Flags ) static CShaderParam Name( #Name, Type, Frequency, HelpString, Flags );
#define SHADER_PARAM( Name, Type, Frequency, HelpString )			   static CShaderParam Name( #Name, Type, Frequency, HelpString, 0 );
#define BEGIN_SHADER_PARAMS
#define END_SHADER_PARAMS

//-----------------------------------------------------------------------------
// Helper macros to declare resources sections (constant buffers, textures, samplers, etc)
//-----------------------------------------------------------------------------
#define SHADER_BUFFER( Name, BindSet, BindSlot, DataType, UsageFlags ) static CShaderBufferInfo Name( BindSet, STUDIO_RESOURCE_BINDING_SLOT_FREE_BEGIN + BindSlot, sizeof( SHADER_BUFFER_DATA_TYPE( DataType ) ), UsageFlags );
#define SHADER_TEXTURE_SAMPLER( Name, BindSet, BindSlot )			   static CShaderTextureSamplerInfo Name( BindSet, STUDIO_RESOURCE_BINDING_SLOT_FREE_BEGIN + BindSlot );
#define BEGIN_SHADER_RESOURCES
#define END_SHADER_RESOURCES

//-----------------------------------------------------------------------------
// Helper macros to declare shader globals
// NOTE: Everything StudioAPI-owned declared here must be created in `SHADER_INIT` and
// released in `SHADER_SHUTDOWN`
//-----------------------------------------------------------------------------
#define BEGIN_SHADER_GLOBALS
#define END_SHADER_GLOBALS

//-----------------------------------------------------------------------------
// Helper macros to declare shader caches
//-----------------------------------------------------------------------------
#define BEGIN_SHADER_CACHES static const char* s_pShaderCacheNames[] = {
#define END_SHADER_CACHES \
	}                     \
	;
#define SHADER_CACHE( Name ) Name,

//-----------------------------------------------------------------------------
// Helper macros to declare per-material context data section
//-----------------------------------------------------------------------------
#define BEGIN_SHADER_PERMATERIAL_CONTEXTDATA                           \
	class CPerMaterialContextData : public CBasePerMaterialContextData \
	{                                                                  \
	public:                                                            \
		using CBaseClass = CBasePerMaterialContextData;                \
		using CThisClass = CPerMaterialContextData;
#define SHADER_PERMATERIAL_CONTEXTDATA_UPDATE			 virtual void OnUpdate( IMaterialVar** pParams ) override
#define SHADER_PERMATERIAL_CONTEXTDATA_BARRIER			 virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const override
#define SHADER_PERMATERIAL_CONTEXTDATA_INIT_STUDIOAPI	 virtual void InitStudioAPI() override
#define SHADER_PERMATERIAL_CONTEXTDATA_UPDATE_STUDIOAPI	 virtual void UpdateStudioAPI() override
#define SHADER_PERMATERIAL_CONTEXTDATA_RELEASE_STUDIOAPI virtual void ReleaseStudioAPI() override
#define END_SHADER_PERMATERIAL_CONTEXTDATA                                                              \
	}                                                                                                   \
	;                                                                                                   \
	class CShader : public CBaseShader                                                                  \
	{                                                                                                   \
	public:                                                                                             \
		virtual CRefPtr<IPerMaterialContextData> CreatePerMaterialContextData() const override          \
		{                                                                                               \
			return new CPerMaterialContextData();                                                       \
		}                                                                                               \
		virtual const char* GetName() const override                                                    \
		{                                                                                               \
			return s_pName;                                                                             \
		}                                                                                               \
		virtual const char* GetHelp() const override                                                    \
		{                                                                                               \
			return s_pHelpString;                                                                       \
		}                                                                                               \
		virtual uint32 GetFlags() const override                                                        \
		{                                                                                               \
			return s_Flags;                                                                             \
		}                                                                                               \
		virtual uint32 GetNumParams( shaderParamFrequency_t frequency ) const override                  \
		{                                                                                               \
			Assert( frequency < SHADER_PARAM_NUM_FREQUENCIES );                                         \
			return (uint32)s_ShaderParams[(uint32)frequency].size();                                    \
		}                                                                                               \
		virtual shaderParam_t GetParam( shaderParamFrequency_t frequency, uint32 index ) const override \
		{                                                                                               \
			Assert( frequency < SHADER_PARAM_NUM_FREQUENCIES );                                         \
			Assert( index < (uint32)s_ShaderParams[(uint32)frequency].size() );                         \
			return s_ShaderParams[(uint32)frequency][index];                                            \
		}                                                                                               \
		virtual uint32 GetNumCacheNames() const override                                                \
		{                                                                                               \
			return ARRAYSIZE( s_pShaderCacheNames );                                                    \
		}                                                                                               \
		virtual const char* GetCacheName( uint32 index ) const override                                 \
		{                                                                                               \
			Assert( index < ARRAYSIZE( s_pShaderCacheNames ) );                                         \
			return s_pShaderCacheNames[index];                                                          \
		}
#define DECLARE_SHADER_PERMATERIAL_CONTEXTDATA( Name ) CPerMaterialContextData* Name = (CPerMaterialContextData*)drawParams.pPerMaterialContextData;

//-----------------------------------------------------------------------------
// Helper macros to implement shader functions
//-----------------------------------------------------------------------------
#define SHADER_INIT						virtual void OnInitInstance() override
#define SHADER_SHUTDOWN					virtual void OnShutdownInstance() override
#define SHADER_INIT_PERMATERIAL_PARAMS	virtual void InitDefaultParams( IMaterialVar** pParams ) const override
#define SHADER_INIT_PERDRAW_PARAMS		virtual void InitDefaultParams( shaderPerDrawVar_t* pParams ) const override
#define SHADER_INIT_DEFAULT_RENDERSTATE virtual void OnInitDefaultRenderState( studioRenderState_t& renderState ) const override
#define SHADER_FALLBACK					virtual const char* GetFallbackShader() const override
#define SHADER_SELECT_COMBO				virtual void R_SelectCombo( const shaderDrawParams_t& drawParams, shaderComboInfo_t& comboInfo ) override
#define SHADER_BIND						virtual void R_OnBind( IStudioAPICmdList* pStudioAPICmdList, const shaderDrawParams_t& drawParams ) override

//-----------------------------------------------------------------------------
// Helper macros to set shader caches and a render state
//-----------------------------------------------------------------------------
#define DECLARE_VERTEX_SHADER( Name )	C_##Name##_vertex_Index __vertexShaderIndex;
#define DECLARE_HULL_SHADER( Name )		C_##Name##_hull_Index __hullShaderIndex;
#define DECLARE_DOMAIN_SHADER( Name )	C_##Name##_domain_Index __domainShaderIndex;
#define DECLARE_GEOMETRY_SHADER( Name ) C_##Name##_geometry_Index __geometryShaderIndex;
#define DECLARE_PIXEL_SHADER( Name )	C_##Name##_pixel_Index __pixelShaderIndex;
#define DECLARE_COMPUTE_SHADER( Name )	C_##Name##_compute_Index __computeShaderIndex;

#define SET_VERTEX_FACTORY( Name, pVertexFactory ) __vertexShaderIndex.SetVertexFactory( pVertexFactory );
#define SET_VERTEX_SHADER_COMBO( Name, Value )	   __vertexShaderIndex.Set##Name( Value );
#define SET_HULL_SHADER_COMBO( Name, Value )	   __hullShaderIndex.Set##Name( Value );
#define SET_DOMAIN_SHADER_COMBO( Name, Value )	   __domainShaderIndex.Set##Name( Value );
#define SET_GEOMETRY_SHADER_COMBO( Name, Value )   __geometryShaderIndex.Set##Name( Value );
#define SET_PIXEL_SHADER_COMBO( Name, Value )	   __pixelShaderIndex.Set##Name( Value );
#define SET_COMPUTE_SHADER_COMBO( Name, Value )	   __computeShaderIndex.Set##Name( Value );

#define SET_VERTEX_SHADER( Name )	comboInfo.cacheIds[STUDIOAPI_SHADER_TYPE_VERTEX] = __vertexShaderIndex.GetIndex();
#define SET_HULL_SHADER( Name )		comboInfo.cacheIds[STUDIOAPI_SHADER_TYPE_HULL] = __hullShaderIndex.GetIndex();
#define SET_DOMAIN_SHADER( Name )	comboInfo.cacheIds[STUDIOAPI_SHADER_TYPE_DOMAIN] = __domainShaderIndex.GetIndex();
#define SET_GEOMETRY_SHADER( Name ) comboInfo.cacheIds[STUDIOAPI_SHADER_TYPE_GEOMETRY] = __geometryShaderIndex.GetIndex();
#define SET_PIXEL_SHADER( Name )	comboInfo.cacheIds[STUDIOAPI_SHADER_TYPE_PIXEL] = __pixelShaderIndex.GetIndex();
#define SET_COMPUTE_SHADER( Name )	comboInfo.cacheIds[STUDIOAPI_SHADER_TYPE_COMPUTE] = __computeShaderIndex.GetIndex();
#define SET_RENDER_STATE( Index )	comboInfo.renderStateIdx = ( Index );

//-----------------------------------------------------------------------------
// Base per-material context data
// NOTE: Shaders can keep per material data in classes descended from this
//-----------------------------------------------------------------------------
class CBasePerMaterialContextData : public CRefCounted<IPerMaterialContextData>, public CStudioRenderResource<IStudioRenderResource>
{
public:
	// IPerMaterialContextData interface
	// Update the per-material context data from material vars
	// NOTE: `pParams` must be size equal to shader parameters
	virtual void Update( IMaterialVar** pParams ) override;

	// Place barriers into a command list
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const override;

protected:
	// IRefCounted interface
	virtual void FinalRelease() override;
	virtual void OnUpdate( IMaterialVar** pParams );
};

//-----------------------------------------------------------------------------
// Base shader
//-----------------------------------------------------------------------------
class CBaseShader : public IShader
{
public:
	// IShader interface
	// Initialize and shutdown functions
	virtual void Init( const shaderInitParams_t& shaderInitParams ) override;
	virtual void InitDefaultParams( IMaterialVar** pParams ) const override;
	virtual void InitDefaultParams( shaderPerDrawVar_t* pParams ) const override;
	virtual void Shutdown() override;

	// Bind the shader's own resources (textures, samplers, constant buffers, etc) into the command list
	virtual void R_Bind( IStudioAPICmdList* pStudioAPICmdList, const shaderDrawParams_t& drawParams ) override;

	// Resolve (baking if needed) the render pipeline for the current combo
	// NOTE: `pVertexFactory` can be NULL when the shader generates its own geometry
	virtual IStudioAPIRenderPipeline* R_ResolveRenderPipeline( const shaderDrawParams_t& drawParams, studioRenderPassType_t renderPassType ) override;

	virtual uint32					  FindParamIndex( shaderParamFrequency_t frequency, const char* pName ) const override;
	virtual const shaderPerDrawVar_t* GetDefaultPerDrawVars() const override;
	virtual const char*				  GetFallbackShader() const override;  // Returns NULL when no have a fallback shader

	CBaseShader();

protected:
	struct shaderComboInfo_t
	{
		uint64 renderStateIdx;	// Index 0 is the shader's default render state and is already selected by default
		uint64 cacheIds[STUDIOAPI_SHADER_NUM_DRAW_TYPES];
	};

	virtual void OnInitInstance();
	virtual void OnShutdownInstance();
	virtual void OnInitDefaultRenderState( studioRenderState_t& renderState ) const;
	virtual void R_SelectCombo( const shaderDrawParams_t& drawParams, shaderComboInfo_t& comboInfo ) = 0;
	virtual void R_OnBind( IStudioAPICmdList* pStudioAPICmdList, const shaderDrawParams_t& drawParams );

	const studioRenderState_t& GetDefaultRenderState() const;
	uint64					   FindOrCreateRenderState( const studioRenderState_t& renderState );

private:
	using shaderPerDrawVars_t  = eastl::vector<shaderPerDrawVar_t>;
	using studioRenderStates_t = eastl::vector<studioRenderState_t>;
	using paramIdsDict_t	   = eastl::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t>;
	using renderStatesDict_t   = eastl::unordered_map<hash, uint64>;
	struct shaderCacheInfoInternal_t
	{
		bool   bValid;		 // Is valid the information
		uint64 scale;		 // Combination scale
		uint64 indexOffset;	 // Cache index offset in the shader manager
	};

	void   InitDefaultRenderState();
	void   ValidateDrawParams( const shaderDrawParams_t& drawParams ) const;
	uint64 GetShaderComboIndex( const uint64* pCacheIds ) const;  // pCacheIds must be array size STUDIOAPI_SHADER_NUM_DRAW_TYPES

	CRefPtr<IStudioRenderPipelineSet> pStudioRenderPipelineSet;
	shaderCacheInfoInternal_t		  cacheInfos[STUDIOAPI_SHADER_NUM_DRAW_TYPES];
	shaderPerDrawVars_t				  defaultPerDrawVars;
	studioRenderStates_t			  renderStates;
	paramIdsDict_t					  paramIdsDict[SHADER_PARAM_NUM_FREQUENCIES];
	renderStatesDict_t				  renderStatesDict;
};

#include "materialsystem/shaderlib/shader_base.inl"
