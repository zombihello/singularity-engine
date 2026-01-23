#pragma once
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/istudio_renderpipelineset.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/ishader.h"
#include "resourcesystem/iresourcesystem.h"

//-----------------------------------------------------------------------------
// Helper macros to begin/end implement a shader
//-----------------------------------------------------------------------------
#define BEGIN_SHADER_FLAGS( Name, HelpString, Flags )                                                                                             \
	namespace Name                                                                                                                                \
	{                                                                                                                                             \
		static const char*					s_pHelpString = HelpString;                                                                           \
		static const char*					s_pName		  = #Name;                                                                                \
		static uint32						s_Flags		  = Flags;                                                                                \
		static uint32						s_NumBuffers  = 0;                                                                                    \
		static eastl::vector<shaderParam_t> s_ShaderParams;                                                                                       \
		class CShaderParam                                                                                                                        \
		{                                                                                                                                         \
		public:                                                                                                                                   \
			CShaderParam( const char* pName, shaderParamType_t type, const char* pHelpString, uint32 flags )                                      \
				: info( s_ShaderParams.emplace_back() )                                                                                           \
				, index( (uint32)s_ShaderParams.size() - 1 )                                                                                      \
			{                                                                                                                                     \
				Assert( index == (uint32)s_ShaderParams.size() - 1 );                                                                             \
				info.pName		 = pName;                                                                                                         \
				info.type		 = type;                                                                                                          \
				info.pHelpString = pHelpString;                                                                                                   \
				info.flags		 = flags;                                                                                                         \
			}                                                                                                                                     \
			operator uint32()                                                                                                                     \
			{                                                                                                                                     \
				return index;                                                                                                                     \
			}                                                                                                                                     \
			operator shaderParam_t()                                                                                                              \
			{                                                                                                                                     \
				return info;                                                                                                                      \
			}                                                                                                                                     \
			FORCEINLINE const char* GetName() const                                                                                               \
			{                                                                                                                                     \
				return info.pName;                                                                                                                \
			}                                                                                                                                     \
			FORCEINLINE shaderParamType_t GetType() const                                                                                         \
			{                                                                                                                                     \
				return info.type;                                                                                                                 \
			}                                                                                                                                     \
			FORCEINLINE const char* GetHelp() const                                                                                               \
			{                                                                                                                                     \
				return info.pHelpString;                                                                                                          \
			}                                                                                                                                     \
			FORCEINLINE uint32 GetFlags() const                                                                                                   \
			{                                                                                                                                     \
				return info.flags;                                                                                                                \
			}                                                                                                                                     \
                                                                                                                                                  \
		private:                                                                                                                                  \
			shaderParam_t& info;                                                                                                                  \
			uint32		   index;                                                                                                                 \
		};                                                                                                                                        \
		class CShaderBufferInfo                                                                                                                   \
		{                                                                                                                                         \
		public:                                                                                                                                   \
			CShaderBufferInfo( uint32 bindSet, uint32 bindSlot, uint32 size, uint32 usageFlags )                                                  \
				: size( size )                                                                                                                    \
				, usageFlags( usageFlags )                                                                                                        \
				, bindSet( bindSet )                                                                                                              \
				, bindSlot( bindSlot )                                                                                                            \
				, index( s_NumBuffers++ )                                                                                                         \
			{                                                                                                                                     \
			}                                                                                                                                     \
			operator uint32()                                                                                                                     \
			{                                                                                                                                     \
				return index;                                                                                                                     \
			}                                                                                                                                     \
			FORCEINLINE uint32 GetSize() const                                                                                                    \
			{                                                                                                                                     \
				return size;                                                                                                                      \
			}                                                                                                                                     \
			FORCEINLINE uint32 GetIndex() const                                                                                                   \
			{                                                                                                                                     \
				return index;                                                                                                                     \
			}                                                                                                                                     \
			FORCEINLINE void UpdateBuffer( IStudioAPICmdContext* pStudioAPICmdContext, byte* pData, TRefPtr<IStudioAPIBuffer>& pStudioAPIBuffer ) \
			{                                                                                                                                     \
				/** If we haven't a buffer or size isn't equal create a new buffer */                                                             \
				if ( !pStudioAPIBuffer || pStudioAPIBuffer->GetSize() != size )                                                                   \
				{                                                                                                                                 \
					pStudioAPIBuffer = g_pStudioAPI->CreateBuffer( pData, size, size, usageFlags );                                               \
				}                                                                                                                                 \
				/** Otherwise update it */                                                                                                        \
				else                                                                                                                              \
				{                                                                                                                                 \
					pStudioAPIBuffer->UpdateData( pStudioAPICmdContext, pData, size );                                                            \
				}                                                                                                                                 \
			}                                                                                                                                     \
			FORCEINLINE void SetConstantBuffer( IStudioAPICmdList* pStudioAPICmdList, IStudioAPIBuffer* pStudioAPIBuffer ) const                  \
			{                                                                                                                                     \
				pStudioAPICmdList->SetConstantBuffer( bindSet, bindSlot, pStudioAPIBuffer );                                                      \
			}                                                                                                                                     \
                                                                                                                                                  \
		private:                                                                                                                                  \
			uint32 size;                                                                                                                          \
			uint32 usageFlags; /* see studioAPIBufferUsageFlag_t */                                                                               \
			uint32 bindSet;                                                                                                                       \
			uint32 bindSlot;                                                                                                                      \
			uint32 index;                                                                                                                         \
		};                                                                                                                                        \
		class CShaderTextureSamplerInfo                                                                                                           \
		{                                                                                                                                         \
		public:                                                                                                                                   \
			CShaderTextureSamplerInfo( uint32 bindSet, uint32 bindSlot )                                                                          \
				: bindSet( bindSet )                                                                                                              \
				, bindSlot( bindSlot )                                                                                                            \
			{                                                                                                                                     \
			}                                                                                                                                     \
			FORCEINLINE void SetTexture( IStudioAPICmdList* pStudioAPICmdList, IStudioAPITexture* pStudioAPITexture ) const                       \
			{                                                                                                                                     \
				pStudioAPICmdList->SetTexture( bindSet, bindSlot, pStudioAPITexture );                                                            \
			}                                                                                                                                     \
			FORCEINLINE void SetSampler( IStudioAPICmdList* pStudioAPICmdList, IStudioAPISampler* pStudioAPISampler ) const                       \
			{                                                                                                                                     \
				pStudioAPICmdList->SetSampler( bindSet, bindSlot, pStudioAPISampler );                                                            \
			}                                                                                                                                     \
                                                                                                                                                  \
		private:                                                                                                                                  \
			uint32 bindSet;                                                                                                                       \
			uint32 bindSlot;                                                                                                                      \
		};

#define BEGIN_SHADER( Name, HelpString ) BEGIN_SHADER_FLAGS( Name, HelpString, 0 )
#define END_SHADER                   \
	}                                \
	;                                \
	static CShader s_ShaderInstance; \
	}  // Namespace

//-----------------------------------------------------------------------------
// Helper macros to declare shader parameters
//-----------------------------------------------------------------------------
#define SHADER_PARAM_FLAGS( Name, Type, HelpString, Flags ) static CShaderParam Name( #Name, Type, HelpString, Flags );
#define SHADER_PARAM( Name, Type, HelpString )				static CShaderParam Name( #Name, Type, HelpString, 0 );
#define BEGIN_SHADER_PARAMS
#define END_SHADER_PARAMS                                               \
	class CShader : public CBaseShader                                  \
	{                                                                   \
	public:                                                             \
		virtual const char* GetName() const override                    \
		{                                                               \
			return s_pName;                                             \
		}                                                               \
		virtual const char* GetHelp() const override                    \
		{                                                               \
			return s_pHelpString;                                       \
		}                                                               \
		virtual uint32 GetFlags() const override                        \
		{                                                               \
			return s_Flags;                                             \
		}                                                               \
		virtual uint32 GetNumParams() const override                    \
		{                                                               \
			return (uint32)s_ShaderParams.size();                       \
		}                                                               \
		virtual shaderParam_t GetParam( uint32 index ) const override   \
		{                                                               \
			Assert( index < (uint32)s_ShaderParams.size() );            \
			return s_ShaderParams[index];                               \
		}                                                               \
		virtual uint32 GetNumCacheNames() const override                \
		{                                                               \
			return ARRAYSIZE( s_pShaderCacheNames );                    \
		}                                                               \
		virtual const char* GetCacheName( uint32 index ) const override \
		{                                                               \
			Assert( index < ARRAYSIZE( s_pShaderCacheNames ) );         \
			return s_pShaderCacheNames[index];                          \
		}                                                               \
		virtual uint32 GetNumBuffers() const override                   \
		{                                                               \
			return s_NumBuffers;                                        \
		}

//-----------------------------------------------------------------------------
// Helper macros to declare resources sections (constant buffers, textures, samplers, etc)
//-----------------------------------------------------------------------------
#define SHADER_BUFFER( Name, BindSet, BindSlot, Struct, UsageFlags ) static CShaderBufferInfo Name( BindSet, BindSlot, sizeof( Struct ), UsageFlags );
#define SHADER_TEXTURE_SAMPLER( Name, BindSet, BindSlot )			 static CShaderTextureSamplerInfo Name( BindSet, BindSlot );
#define BEGIN_SHADER_RESOURCES
#define END_SHADER_RESOURCES

//-----------------------------------------------------------------------------
// Helper macros to declare shader caches
//-----------------------------------------------------------------------------
#define BEGIN_SHADER_CACHES static const char* s_pShaderCacheNames[] = {
#define END_SHADER_CACHES \
	}                     \
	;
#define SHADER_CACHE( Name ) Name,

//-----------------------------------------------------------------------------
// Helper macros to implement shader functions
//-----------------------------------------------------------------------------
#define SHADER_INIT			  virtual void OnInitInstance() override
#define SHADER_INIT_PARAMS	  virtual void InitDefaultParams( IMaterialVar** pParams ) const override
#define SHADER_FALLBACK		  virtual const char* GetFallbackShader() const override
#define SHADER_SELECT_COMBO	  virtual void R_SelectCombo( IMaterialVar** pParams, shaderComboInfo_t& comboInfo ) override
#define SHADER_DRAW			  virtual void R_OnDraw( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers = NULL ) override
#define SHADER_UPDATE_BUFFERS virtual void R_UpdateBuffers( IStudioAPICmdContext* pStudioAPICmdContext, TRefPtr<IStudioAPIBuffer>* pStudioAPIBuffers, IMaterialVar** pParams ) const override
#define SHADER_BARRIER		  virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers ) const override

//-----------------------------------------------------------------------------
// Helper macros to set shader caches
//-----------------------------------------------------------------------------
#define DECLARE_VERTEX_SHADER( Name )	C_##Name##_vertex_Index __vertexShaderIndex;
#define DECLARE_HULL_SHADER( Name )		C_##Name##_hull_Index __hullShaderIndex;
#define DECLARE_DOMAIN_SHADER( Name )	C_##Name##_domain_Index __domainShaderIndex;
#define DECLARE_GEOMETRY_SHADER( Name ) C_##Name##_geometry_Index __geometryShaderIndex;
#define DECLARE_PIXEL_SHADER( Name )	C_##Name##_pixel_Index __pixelShaderIndex;
#define DECLARE_COMPUT_SHADER( Name )	C_##Name##_compute_Index __computeShaderIndex;

#define SET_VERTEX_TYPE( VertexType )			 comboInfo.vertexType = VertexType;
#define SET_VERTEX_SHADER_COMBO( Name, Value )	 __vertexShaderIndex.Set##Name( Value );
#define SET_HULL_SHADER_COMBO( Name, Value )	 __hullShaderIndex.Set##Name( Value );
#define SET_DOMAIN_SHADER_COMBO( Name, Value )	 __domainShaderIndex.Set##Name( Value );
#define SET_GEOMETRY_SHADER_COMBO( Name, Value ) __geometryShaderIndex.Set##Name( Value );
#define SET_PIXEL_SHADER_COMBO( Name, Value )	 __pixelShaderIndex.Set##Name( Value );
#define SET_COMPUTE_SHADER_COMBO( Name, Value )	 __computeShaderIndex.Set##Name( Value );

#define SET_VERTEX_SHADER( Name )	comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_VERTEX] = __vertexShaderIndex.GetIndex();
#define SET_HULL_SHADER( Name )		comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_HULL] = __hullShaderIndex.GetIndex();
#define SET_DOMAIN_SHADER( Name )	comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_DOMAIN] = __domainShaderIndex.GetIndex();
#define SET_GEOMETRY_SHADER( Name ) comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_GEOMETRY] = __geometryShaderIndex.GetIndex();
#define SET_PIXEL_SHADER( Name )	comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_PIXEL] = __pixelShaderIndex.GetIndex();
#define SET_COMPUTE_SHADER( Name )	comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_COMPUTE] = __computeShaderIndex.GetIndex();

//-----------------------------------------------------------------------------
// Base shader
//-----------------------------------------------------------------------------
class CBaseShader : public IShader
{
public:
	CBaseShader();

	// Initialize and shutdown functions
	virtual void Init( const shaderInitParams_t& shaderInitParams ) override;
	virtual void InitDefaultParams( IMaterialVar** pParams ) const override;
	virtual void Shutdown() override;

	// NOTE: pStudioAPIBuffers and pParams must be array size equal to shader buffer and parameters count
	virtual void R_UpdateBuffers( IStudioAPICmdContext* pStudioAPICmdContext, TRefPtr<IStudioAPIBuffer>* pStudioAPIBuffers, IMaterialVar** pParams ) const override;
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers ) const override;
	virtual void R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers = NULL ) override;

	// Get a fallback shader
	// Returns NULL when no have a fallback shader
	virtual const char* GetFallbackShader() const override;

protected:
	struct shaderComboInfo_t
	{
		uint64			   cacheIndices[STUDIOAPI_SHADER_NUM_DRAW_TYPES];
		studioVertexType_t vertexType;
	};

	virtual void OnInitInstance();

	// NOTE: pStudioAPIBuffers and pParams must be array size equal to shader buffer and parameters count
	virtual void R_SelectCombo( IMaterialVar** pParams, shaderComboInfo_t& comboInfo )												   = 0;
	virtual void R_OnDraw( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers = NULL ) = 0;

private:
	struct shaderCacheInfoInternal_t
	{
		bool   bValid;		 // Is valid the information
		uint64 scale;		 // Combination scale
		uint64 indexOffset;	 // Cache index offset in the shader manager
	};

	// pCacheIndices must be array size STUDIOAPI_SHADER_NUM_DRAW_TYPES
	uint64 GetPipelineIndex( const uint64* pCacheIndices ) const;

	TRefPtr<IStudioRenderPipelineSet> pStudioRenderPipelineSet;
	shaderCacheInfoInternal_t		  cacheInfos[STUDIOAPI_SHADER_NUM_DRAW_TYPES];
};

#include "materialsystem/shaderlib/shader_base.inl"
