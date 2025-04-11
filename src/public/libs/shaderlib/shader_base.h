/**
 * @file
 * @addtogroup shaderlib shaderlib
 */

#ifndef SHADER_BASE_H
#define SHADER_BASE_H

#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "studiorender/istudio_renderpipelineset.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/ishader.h"
#include "resourcesystem/iresourcesystem.h"

/**
 * @ingroup shaderslibrary
 * @brief Macro of begin implementation shader with custom flags
 * 
 * @param Name			Shader name
 * @param HelpString	Help message
 * @param Flags			Shader flags (see shaderFlags_t)
 */
#define BEGIN_SHADER_FLAGS( Name, HelpString, Flags ) \
	namespace Name \
	{ \
		/** Shader help message */ \
		static const achar*										s_pHelpString = HelpString; \
		/** Shader name */ \
		static const achar*										s_pName = #Name; \
		/** Shader flags (see shaderFlags_t) */ \
		static uint32											s_Flags = Flags; \
		/** Shader buffer count */ \
		static uint32											s_NumBuffers = 0; \
		/** Shader parameters */ \
		static std::vector<shaderParam_t>						s_ShaderParams; \
		/** Shader parameter class */ \
		class CShaderParam \
		{ \
		public: \
			/** Constructor */ \
			CShaderParam( const achar* pName, shaderParamType_t type, const achar* pHelpString, uint32 flags ) \
				: info( s_ShaderParams.emplace_back() ) \
				, index( ( uint32 )s_ShaderParams.size() - 1 ) \
			{ \
				Assert( index == ( uint32 )s_ShaderParams.size() - 1 ); \
				info.pName			= pName; \
				info.type			= type; \
				info.pHelpString	= pHelpString; \
				info.flags			= flags; \
			} \
			/** Cast a shader parameter into uint32 */ \
			operator uint32() \
			{ \
				return index; \
			} \
			/** Cast a shader parameter into shaderParam_t */ \
			operator shaderParam_t() \
			{ \
				return info; \
			} \
			/** Get parameter name */ \
			FORCEINLINE const achar* GetName() const \
			{ \
				return info.pName; \
			} \
			/** Get parameter type */ \
			FORCEINLINE shaderParamType_t GetType() const \
			{ \
				return info.type; \
			} \
			/** Get help info about parameter */ \
			FORCEINLINE const achar* GetHelp() const \
			{ \
				return info.pHelpString; \
			} \
			/** Get parameter flags */ \
			FORCEINLINE uint32 GetFlags() const \
			{ \
				return info.flags; \
			} \
		private: \
			shaderParam_t&	info;	/**< Shader parameters */ \
			uint32			index;	/**< Index in s_ShaderParams */ \
		}; \
		/** Shader buffer info class */ \
		class CShaderBufferInfo \
		{ \
		public: \
			/** Constructor */ \
			CShaderBufferInfo( uint32 bindSet, uint32 bindSlot, uint32 size, uint32 usageFlags ) \
				: size( size ) \
				, usageFlags( usageFlags ) \
				, bindSet( bindSet ) \
				, bindSlot( bindSlot ) \
				, index( s_NumBuffers++ ) \
			{} \
			/** Cast a shader buffer into uint32 */ \
			operator uint32() \
			{ \
				return index; \
			} \
			/** Get buffer size */ \
			FORCEINLINE uint32 GetSize() const \
			{ \
				return size; \
			} \
			/** Get buffer index */ \
			FORCEINLINE uint32 GetIndex() const \
			{ \
				return index; \
			} \
			/** Update a buffer */ \
			FORCEINLINE void UpdateBuffer( IStudioAPICmdContext* pStudioAPICmdContext, byte* pData, TRefPtr<IStudioAPIBuffer>& pStudioAPIBuffer ) \
			{ \
				/** If we haven't a buffer or size isn't equal create a new buffer */ \
				if ( !pStudioAPIBuffer || pStudioAPIBuffer->GetSize() != size ) \
				{ \
					pStudioAPIBuffer = g_pStudioAPI->CreateBuffer( pData, size, size, usageFlags ); \
				} \
				/** Otherwise update it */ \
				else \
				{ \
					pStudioAPIBuffer->UpdateData( pStudioAPICmdContext, pData, size ); \
				} \
			} \
			/** Set constant buffer */ \
			FORCEINLINE void SetConstantBuffer( IStudioAPICmdList* pStudioAPICmdList, IStudioAPIBuffer* pStudioAPIBuffer ) const \
			{ \
				pStudioAPICmdList->SetConstantBuffer( bindSet, bindSlot, pStudioAPIBuffer ); \
			} \
		private: \
			uint32		size;		/**< Buffer size */ \
			uint32		usageFlags;	/**< Usage flags (see studioAPIBufferUsageFlag_t) */ \
			uint32		bindSet;	/**< Bind set */ \
			uint32		bindSlot;	/**< Bind slot */ \
			uint32		index;		/**< Buffer index */ \
		}; \
		/** Shader texture sampler info class */ \
		class CShaderTextureSamplerInfo \
		{ \
		public: \
			/** Constructor */ \
			CShaderTextureSamplerInfo( uint32 bindSet, uint32 bindSlot ) \
				: bindSet( bindSet ) \
				, bindSlot( bindSlot ) \
			{} \
			/** Set texture */ \
			FORCEINLINE void SetTexture( IStudioAPICmdList* pStudioAPICmdList,  IStudioAPITexture* pStudioAPITexture ) const \
			{ \
				pStudioAPICmdList->SetTexture( bindSet, bindSlot, pStudioAPITexture ); \
			} \
			/** Set sampler */ \
			FORCEINLINE void SetSampler( IStudioAPICmdList* pStudioAPICmdList,  IStudioAPISampler* pStudioAPISampler ) const \
			{ \
				pStudioAPICmdList->SetSampler( bindSet, bindSlot, pStudioAPISampler ); \
			} \
		private: \
			uint32		bindSet;	/**< Bind set */ \
			uint32		bindSlot;	/**< Bind slot */ \
		};

/**
 * @ingroup shaderslibrary
 * @brief Macro of begin implementation shader
 *
 * @param Name			Shader name
 * @param HelpString	Help message
 */
#define BEGIN_SHADER( Name, HelpString ) \
	BEGIN_SHADER_FLAGS( Name, HelpString, 0 )

/**
 * @ingroup shaderslibrary
 * @brief Macro of begin shader parameters section
 */
#define BEGIN_SHADER_PARAMS

/**
 * @ingroup shaderslibrary
 * @brief Macro of create shader parameter with custom flags
 * 
 * @param Name			Parameter name
 * @param Type			Parameter type
 * @param HelpString	Help message for this parameter
 * @param Flags			Parameter flags
 */
#define SHADER_PARAM_FLAGS( Name, Type, HelpString, Flags ) \
	static CShaderParam Name( #Name, Type, HelpString, Flags );

/**
 * @ingroup shaderslibrary
 * @brief Macro of create shader parameter
 * 
 * @param Name			Parameter name
 * @param Type			Parameter type
 * @param Help			Help message for this parameter
 */
#define SHADER_PARAM( Name, Type, HelpString ) \
	static CShaderParam Name( #Name, Type, HelpString, 0 );

/**
 * @ingroup shaderslibrary
 * @brief Macro of end shader parameters section
 */
#define END_SHADER_PARAMS \
	class CShader : public CBaseShader \
	{ \
	public: \
		/** Get shader name */ \
		virtual const achar* GetName() const override \
		{ \
			return s_pName; \
		} \
		/** Get shader help message */ \
		virtual const achar* GetHelp() const override \
		{ \
			return s_pHelpString; \
		} \
		/** Get shader flags */ \
		virtual uint32 GetFlags() const override \
		{ \
			return s_Flags; \
		} \
		/** Get number of shader parameters */ \
		virtual uint32 GetNumParams() const override \
		{ \
			return ( uint32 )s_ShaderParams.size(); \
		} \
		/** Get shader parameter by index */ \
		virtual shaderParam_t GetParam( uint32 index ) const override \
		{ \
			Assert( index < ( uint32 )s_ShaderParams.size() ); \
			return s_ShaderParams[index]; \
		} \
		/** Get number of shader cache names */ \
		virtual uint32 GetNumCacheNames() const override \
		{ \
			return ARRAYSIZE( s_pShaderCacheNames ); \
		} \
		/** Get shader cache name by index */ \
		virtual const achar* GetCacheName( uint32 index ) const override \
		{ \
			Assert( index < ARRAYSIZE( s_pShaderCacheNames ) ); \
			return s_pShaderCacheNames[index]; \
		} \
		/** Get shader buffer count */ \
		virtual uint32 GetNumBuffers() const override \
		{ \
			return s_NumBuffers; \
		}

/**
 * @ingroup shaderslibrary
 * @brief Macro of begin shader resources section (constant buffers, textures, samplers, etc)
 */
#define BEGIN_SHADER_RESOURCES

/**
 * @ingroup shaderslibrary
 * @brief Macro of create buffer info
 * @param Name			Constant buffer name
 * @param BindSet		Bind set location
 * @param BindSlot		Bind slot location
 * @param Struct		Buffer struct
 * @param UsageFlags	Usage flags (see studioAPIBufferUsageFlag_t)
 */
#define SHADER_BUFFER( Name, BindSet, BindSlot, Struct, UsageFlags ) \
	static CShaderBufferInfo Name( BindSet, BindSlot, sizeof( Struct ), UsageFlags );

/**
 * @ingroup shaderslibrary
 * @brief Macro of create texture or sampler info
 * @param Name			Texture name
 * @param BindSet		Bind set location
 * @param BindSlot		Bind slot location
 */
#define SHADER_TEXTURE_SAMPLER( Name, BindSet, BindSlot ) \
	static CShaderTextureSamplerInfo Name( BindSet, BindSlot );

/**
 * @ingroup shaderslibrary
 * @brief Macro of end shader resources section section
 */
#define END_SHADER_RESOURCES

/**
 * @ingroup shaderslibrary
 * @brief Macro of begin shader caches section
 */
#define BEGIN_SHADER_CACHES \
	/** Shader cache file names */ \
	static const achar* s_pShaderCacheNames[] = {

/**
 * @ingroup shaderslibrary
 * @brief Macro of set shader cache
 * @param Name		Shader cache name
 */
#define SHADER_CACHE( Name ) \
	Name,

/**
 * @ingroup shaderslibrary
 * @brief Macro of end shader caches section
 */
#define END_SHADER_CACHES \
	};

/**
 * @ingroup shaderslibrary
 * @brief Macro of initialize the shader
 */
#define SHADER_INIT \
	/** Initialize a shader instance */ \
	virtual void OnInitInstance() override

/**
 * @ingroup shaderslibrary
 * @brief Macro of initialize parameters to default values
 */
#define SHADER_INIT_PARAMS \
	/** Initialize parameters to default values */ \
	virtual void InitDefaultParams( IMaterialVar** pParams ) const override

/**
 * @ingroup shaderslibrary
 * @brief Macro of set fallback shader
 */
#define SHADER_FALLBACK \
	/** Get fallback shader */ \
	virtual const achar* GetFallbackShader() const override

/**
 * @ingroup shaderslibrary
 * @brief Macro of select a shader combination
 */
#define SHADER_SELECT_COMBO \
	/** Select a shader combination */ \
	virtual void R_SelectCombo( IMaterialVar** pParams, shaderComboInfo_t& comboInfo ) override

/**
 * @ingroup shaderslibrary
 * @brief Macro of draw by a shader
 */
#define SHADER_DRAW \
	/** Prepare shader for draw */ \
	virtual void R_OnDraw( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers = NULL ) override

/**
 * @ingroup shaderslibrary
 * @brief Macro of update shader buffers
 */
#define SHADER_UPDATE_BUFFERS \
	/** Update shader buffers */ \
	virtual void R_UpdateBuffers( IStudioAPICmdContext* pStudioAPICmdContext, TRefPtr<IStudioAPIBuffer>* pStudioAPIBuffers, IMaterialVar** pParams ) const override

/**
 * @ingroup shaderslibrary
 * @brief Macro of place barriers into a list
 */
#define SHADER_BARRIER \
	/** Place barriers into a list */ \
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers ) const override

/**
 * @ingroup shaderslibrary
 * @brief Macro of end implementation shader
 */
#define END_SHADER \
		}; \
		static CShader s_ShaderInstance; \
	} // Namespace

/**
 * @ingroup shaderslibrary
 * @brief Macro of set a vertex type
 * @param VertexType	Vertex type (see studioVertexType_t)
 */
#define SET_VERTEX_TYPE( VertexType ) \
	comboInfo.vertexType = VertexType;

/**
 * @ingroup shaderslibrary
 * @brief Macro to declaration of a vertex shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define DECLARE_VERTEX_SHADER( Name ) \
	C_##Name##_vertex_Index			__vertexShaderIndex;

/**
 * @ingroup shaderslibrary
 * @brief Macro to declaration of a hull shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define DECLARE_HULL_SHADER( Name ) \
	C_##Name##_hull_Index			__hullShaderIndex;

/**
 * @ingroup shaderslibrary
 * @brief Macro to declaration of a domain shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define DECLARE_DOMAIN_SHADER( Name ) \
	C_##Name##_domain_Index			__domainShaderIndex;

/**
 * @ingroup shaderslibrary
 * @brief Macro to declaration of a geometry shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define DECLARE_GEOMETRY_SHADER( Name ) \
	C_##Name##_geometry_Index			__geometryShaderIndex;

/**
 * @ingroup shaderslibrary
 * @brief Macro to declaration of a pixel shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define DECLARE_PIXEL_SHADER( Name ) \
	C_##Name##_pixel_Index			__pixelShaderIndex;

/**
 * @ingroup shaderslibrary
 * @brief Macro to declaration of a compute shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define DECLARE_COMPUT_SHADER( Name ) \
	C_##Name##_compute_Index			__computeShaderIndex;

/**
 * @ingroup shaderslibrary
 * @brief Macro to set a vertex shader flag
 * @param Name		Flag name
 * @param Value		Flag value	
 */
#define SET_VERTEX_SHADER_COMBO( Name, Value ) \
	__vertexShaderIndex.Set##Name( Value );

/**
 * @ingroup shaderslibrary
 * @brief Macro to set a hull shader flag
 * @param Name		Flag name
 * @param Value		Flag value	
 */
#define SET_HULL_SHADER_COMBO( Name, Value ) \
	__hullShaderIndex.Set##Name( Value );

/**
 * @ingroup shaderslibrary
 * @brief Macro to set a domain shader flag
 * @param Name		Flag name
 * @param Value		Flag value	
 */
#define SET_DOMAIN_SHADER_COMBO( Name, Value ) \
	__domainShaderIndex.Set##Name( Value );

/**
 * @ingroup shaderslibrary
 * @brief Macro to set a geometry shader flag
 * @param Name		Flag name
 * @param Value		Flag value	
 */
#define SET_GEOMETRY_SHADER_COMBO( Name, Value ) \
	__geometryShaderIndex.Set##Name( Value );

/**
 * @ingroup shaderslibrary
 * @brief Macro to set a pixel shader flag
 * @param Name		Flag name
 * @param Value		Flag value	
 */
#define SET_PIXEL_SHADER_COMBO( Name, Value ) \
	__pixelShaderIndex.Set##Name( Value );

/**
 * @ingroup shaderslibrary
 * @brief Macro to set a compute shader flag
 * @param Name		Flag name
 * @param Value		Flag value	
 */
#define SET_COMPUTE_SHADER_COMBO( Name, Value ) \
	__computeShaderIndex.Set##Name( Value );

/**
 * @ingroup shaderslibrary
 * @brief Macro of set the vertex shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define SET_VERTEX_SHADER( Name ) \
	comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_VERTEX] = __vertexShaderIndex.GetIndex();

/**
 * @ingroup shaderslibrary
 * @brief Macro of set the hull shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define SET_HULL_SHADER( Name ) \
	comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_HULL] = __hullShaderIndex.GetIndex();

/**
 * @ingroup shaderslibrary
 * @brief Macro of set the domain shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define SET_DOMAIN_SHADER( Name ) \
	comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_DOMAIN] = __domainShaderIndex.GetIndex();

/**
 * @ingroup shaderslibrary
 * @brief Macro of set the geometry shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define SET_GEOMETRY_SHADER( Name ) \
	comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_GEOMETRY] = __geometryShaderIndex.GetIndex();

/**
 * @ingroup shaderslibrary
 * @brief Macro to set the pixel shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define SET_PIXEL_SHADER( Name ) \
	comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_PIXEL] = __pixelShaderIndex.GetIndex();

/**
 * @ingroup shaderslibrary
 * @brief Macro of set the compute shader
 * @param Name		Shader cache name from SHADER_CACHE( ... )
 */
#define SET_COMPUTE_SHADER( Name ) \
	comboInfo.cacheIndices[STUDIOAPI_SHADER_TYPE_COMPUTE] = __computeShaderIndex.GetIndex();

/**
 * @ingroup shaderslibrary
 * @brief Base shader
 */
class CBaseShader : public IShader
{
public:
	/**
	 * @brief Constructor
	 */
	CBaseShader();

	/**
	 * @brief Initialize the shader
	 * @param shaderInitParams		Shader initialize parameters
	 */
	virtual void Init( const shaderInitParams_t& shaderInitParams ) override;

	/**
	 * @brief Shutdown the shader
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Get fallback shader 
	 * @return If StudioAPI isn't support this shader return fallback shader name, otherwise returns NULL
	 */
	virtual const achar* GetFallbackShader() const override;

	/**
	 * @brief Initialize parameters to default values
	 * @param pParams	Material parameters. Must be size equal to shader parameters count
	 */
	virtual void InitDefaultParams( IMaterialVar** pParams ) const override;

	/**
	 * @brief Update shader buffers
	 * @param pStudioAPICmdContext	StudioAPI command context
	 * @param pStudioAPIBuffers		Array of buffer pointers. Must be array size equal to shader buffer count. If pointer is NULL the function create a buffer, otherwise the buffer will be updated
	 * @param pParams				Material parameters. Must be size equal to shader parameters count
	 */
	virtual void R_UpdateBuffers( IStudioAPICmdContext* pStudioAPICmdContext, TRefPtr<IStudioAPIBuffer>* pStudioAPIBuffers, IMaterialVar** pParams ) const override;

	/**
	 * @brief Place barriers into a list
	 * @warning This is only called by the render thread
	 *
	 * @param pStudioAPICmdList		StudioAPI command list
	 * @param pParams				Material parameters. Must be size equal to shader parameters count
	 * @param pStudioAPIBuffers		StudioAPI buffer. Must be size equal to buffer count
	 */
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers ) const override;

	/**
	 * @brief Prepare for draw
	 * @warning This is only called by the render thread
	 *
	 * @param pStudioAPICmdList		StudioAPI command list
	 * @param renderPassType		Render pass type
	 * @param pParams				Material parameters. Must be size equal to shader parameters count
	 * @param pStudioAPIBuffers		StudioAPI buffer. Must be size equal to buffer count
	 */
	virtual void R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers = NULL ) override;

protected:
	/**
	 * @brief Shader combination information
	 */
	struct shaderComboInfo_t
	{
		uint64				cacheIndices[STUDIOAPI_SHADER_NUM_DRAW_TYPES];		/**< Selected shader cache index for each shader type */
		studioVertexType_t	vertexType;											/**< Selected vertex type */
	};

	/**
	 * @brief Initialize a shader instance
	 */
	virtual void OnInitInstance();

	/**
	 * @brief Select a shader combination
	 * @param pParams		Material parameters. Must be size equal to shader parameters count
	 * @param comboInfo		Output shader combination information
	 */
	virtual void R_SelectCombo( IMaterialVar** pParams, shaderComboInfo_t& comboInfo ) = 0;

	/**
	 * @brief Prepare shader for draw
	 * @param pStudioAPICmdList				StudioAPI command list
	 * @param pParams						Material parameters. Must be size equal to shader parameters count
	 * @param pStudioAPIBuffers				StudioAPI buffer. Must be size equal to buffer count
	 */
	virtual void R_OnDraw( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers = NULL ) = 0;

private:
	/**
	 * @brief Shader cache information (internal version)
	 */
	struct shaderCacheInfoInternal_t
	{
		bool		bValid;			/**< Is valid the information */
		uint64		scale;			/**< Combination scale */
		uint64		indexOffset;	/**< Cache index offset in the shader manager */
	};

	/**
	 * @brief Get a pipeline index by cache indices
	 * @param pCacheIndices		Cache indices array, must be array size STUDIOAPI_SHADER_NUM_DRAW_TYPES
	 * @return Return a pipeline index for the cache indices
	 */
	FORCEINLINE uint64 GetPipelineIndex( const uint64* pCacheIndices ) const
	{
		uint64		pipelineIndex = 0;
		for ( uint32 shaderTypeIdx = 0; shaderTypeIdx < STUDIOAPI_SHADER_NUM_DRAW_TYPES; ++shaderTypeIdx )
		{
			pipelineIndex += cacheInfos[shaderTypeIdx].scale * pCacheIndices[shaderTypeIdx];
		}
		return pipelineIndex;
	}

	TRefPtr<IStudioRenderPipelineSet>	pStudioRenderPipelineSet;							/**< Studio render pipeline set */
	shaderCacheInfoInternal_t			cacheInfos[STUDIOAPI_SHADER_NUM_DRAW_TYPES];		/**< Shader cache infos for each shader type */
};

#endif // !SHADER_BASE_H