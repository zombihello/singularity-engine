/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef ISHADER_H
#define ISHADER_H

#include "core/types.h"
#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/studio_renderpasstypes.h"

// Forward declarations
class IStudioAPICmdList;
class IMaterialVar;

/**
 * @ingroup materialsystem
 * @brief Shader flags
 */
enum shaderFlags_t
{
	SHADER_NOT_EDITABLE			= 1 << 1		/**< Shader not editable */
};

/**
 * @ingroup materialsystem
 * @brief Shader parameter flags
 */
enum shaderParamFlags_t
{
	SHADER_PARAM_NOT_EDITABLE	= 1 << 1		/**< Shader parameter not editable */
};

/**
 * @ingroup materialsystem
 * @brief Shader parameter type
 */
enum shaderParamType_t
{
	SHADER_PARAM_TYPE_UNDEFINED,	/**< Undefined type */
	SHADER_PARAM_TYPE_BOOL,			/**< Bool */
	SHADER_PARAM_TYPE_INT,			/**< Integer */
	SHADER_PARAM_TYPE_FLOAT,		/**< Float */
	SHADER_PARAM_TYPE_VECTOR_2D,	/**< Vector 2D */
	SHADER_PARAM_TYPE_VECTOR_3D,	/**< Vector 3D */
	SHADER_PARAM_TYPE_VECTOR_4D,	/**< Vector 4D */
	SHADER_PARAM_TYPE_MATRIX,		/**< Matrix */
	SHADER_PARAM_TYPE_STRING,		/**< String */
	SHADER_PARAM_TYPE_TEXTURE,		/**< Texture */
	SHADER_PARAM_TYPE_MATERIAL		/**< Material */
};

/**
 * @ingroup materialsystem
 * @brief Shader parameter
 */
struct shaderParam_t
{
	const achar*		pName;			/**< Name */
	shaderParamType_t	type;			/**< Type */
	const achar*		pHelpString;	/**< Help message */
	uint32				flags;			/**< Flags (see shaderParamFlags_t) */
};

/**
 * @ingroup materialsystem
 * @brief Shader cache information
 */
struct shaderCacheInfo_t
{
	uint64		numCaches;		/**< Caches number */
	uint64		indexOffset;	/**< Cache index offset in the shader manager */
};

/**
 * @ingroup materialsystem
 * @brief Shader initialize parameters
 */
struct shaderInitParams_t
{
	shaderCacheInfo_t		cacheInfos[STUDIOAPI_SHADER_NUM_DRAW_TYPES];		/**< Shader cache infos for each shader draw type */
};

/**
 * @ingroup materialsystem
 * @brief Shader interface
 */
class IShader
{
public:
	/**
	 * @brief Initialize the shader
	 * @param shaderInitParams		Shader initialize parameters
	 */
	virtual void Init( const shaderInitParams_t& shaderInitParams ) = 0;

	/**
	 * @brief Shutdown the shader
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Initialize parameters to default values
	 * @param pParams	Material parameters. Must be size equal to shader parameters count
	 */
	virtual void InitDefaultParams( IMaterialVar** pParams ) const = 0;

	/**
	 * @brief Update shader buffers
	 * @warning This is only called by the render thread
	 * 
	 * @param pStudioAPICmdContext	StudioAPI command context
	 * @param pStudioAPIBuffers		Array of buffer pointers. Must be array size equal to shader buffer count. If pointer is NULL the function create a buffer, otherwise the buffer will be updated
	 * @param pParams				Material parameters. Must be size equal to shader parameters count
	 */
	virtual void R_UpdateBuffers( IStudioAPICmdContext* pStudioAPICmdContext, TRefPtr<IStudioAPIBuffer>* pStudioAPIBuffers, IMaterialVar** pParams ) const = 0;

	/**
	 * @brief Prepare for draw
	 * @warning This is only called by the render thread
	 *
	 * @param pStudioAPICmdList		StudioAPI command list
	 * @param renderPassType		Render pass type
	 * @param pParams				Material parameters. Must be size equal to shader parameters count
	 * @param pStudioAPIBuffers		StudioAPI buffer. Must be size equal to buffer count
	 */
	virtual void R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers = NULL ) = 0;

	/**
	 * @brief Place barriers into a list
	 * @warning This is only called by the render thread
	 *
	 * @param pStudioAPICmdList		StudioAPI command list
	 * @param pParams				Material parameters. Must be size equal to shader parameters count
	 * @param pStudioAPIBuffers		StudioAPI buffer. Must be size equal to buffer count
	 */
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList, IMaterialVar** pParams, IStudioAPIBuffer** pStudioAPIBuffers ) const = 0;

	/**
	 * @brief Get number of shader parameters
	 * @return Return number of shader parameters
	 */
	virtual uint32 GetNumParams() const = 0;

	/**
	 * @brief Get shader parameter by index
	 * @param index		Parameter index
	 * @return Return shader parameter
	 */
	virtual shaderParam_t GetParam( uint32 index ) const = 0;

	/**
	 * @brief Get number of shader cache names
	 * @return Return number of shader cache names
	 */
	virtual uint32 GetNumCacheNames() const = 0;

	/**
	 * @brief Get shader cache name by index
	 * @param index		Shader cache name index
	 * @return Return shader cache name
	 */
	virtual const achar* GetCacheName( uint32 index ) const = 0;

	/**
	 * @brief Get shader buffer count
	 * @param Return shader buffer count
	 */
	virtual uint32 GetNumBuffers() const = 0;

	/**
	 * @brief Get shader flags
	 * @return Return shader flags (see shaderFlags_t)
	 */
	virtual uint32 GetFlags() const = 0;

	/**
	 * @brief Get shader name
	 * @return Return name of this shader
	 */
	virtual const achar* GetName() const = 0;

	/**
	 * @brief Get shader help message
	 * @return Return help message for this shader 
	 */
	virtual const achar* GetHelp() const = 0;

	/**
	 * @brief Get fallback shader 
	 * @return If StudioAPI isn't support this shader return fallback shader name, otherwise returns NULL
	 */
	virtual const achar* GetFallbackShader() const = 0;
};

#endif // !ISHADER_H