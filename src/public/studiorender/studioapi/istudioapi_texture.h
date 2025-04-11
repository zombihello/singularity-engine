/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_TEXTURE_H
#define ISTUDIOAPI_TEXTURE_H

#include "studiorender/studioapi/istudioapi_resource.h"
#include "studiorender/studioapi/istudioapi_renderpipeline.h"

// Forward declarations
class IStudioAPICmdContext;

/**
 * @ingroup studioapi
 * @brief StudioAPI texture layout
 */
enum studioAPITextureLayout_t
{
	STUDIOAPI_TEXTURE_LAYOUT_UNDEFINED,								/**< Invalid layout (don't preserve contents) */
	STUDIOAPI_TEXTURE_LAYOUT_SHADER_RESOURCE,						/**< Shader resource, read and write */
	STUDIOAPI_TEXTURE_LAYOUT_SHADER_RESOURCE_READONLY,				/**< Shader resource, read only */
	STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_SRC,							/**< Copy a data from */
	STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_DST,							/**< Copy a data to */
	STUDIOAPI_TEXTURE_LAYOUT_COLOR_RENDER_TARGET,					/**< Color render target, write enabled */
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_STENCIL_RENDER_TARGET,			/**< Depth stencil render target, depth/stencil write enabled */
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_READONLY_STENCIL_RENDER_TARGET,	/**< Depth stencil render target, depth read only and stencil write enabled */
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_RENDER_TARGET_STENCIL_READONLY,	/**< Depth stencil render target, depth write enabled and stencil read only */
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_READONLY_STENCIL_READONLY,		/**< Depth stencil render target, depth/stencil read only */
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_RENDER_TARGET,					/**< Depth render target, write enabled */
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_READONLY,						/**< Depth render target, read only */
	STUDIOAPI_TEXTURE_LAYOUT_STENCIL_RENDER_TARGET,					/**< Stencil render target, write enabled */
	STUDIOAPI_TEXTURE_LAYOUT_STENCIL_READONLY,						/**< Stencil render target, read only */
	STUDIOAPI_TEXTURE_LAYOUT_PRESENT								/**< Present the texture on a window */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI pixel format
 */
enum studioAPIPixelFormat_t
{
	STUDIOAPI_PIXEL_FORMAT_UNKNOWN,					/**< Unknown pixel format  */

	// Channel component formats
	STUDIOAPI_PIXEL_FORMAT_RGBA8,					/**< RGBA with 8 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_RG8,						/**< RG with 8 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_R8,						/**< R with 8 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_RGBA16,					/**< RGBA with 16 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_RG16,					/**< RG with 16 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_R16,						/**< R with 16 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_RGBA32F,					/**< RGBA with float 32 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_RG32F,					/**< RG with float 32 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_R32F,					/**< R with float 32 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_RGBA16F,					/**< RGBA with float 16 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_RG16F,					/**< RG with float 16 bit per channel */
	STUDIOAPI_PIXEL_FORMAT_R16F,					/**< R with float 16 bit per channel */

	// Compression formats
	STUDIOAPI_PIXEL_FORMAT_BC1,						/**< BC1 (DXT1) compression */
	STUDIOAPI_PIXEL_FORMAT_BC2,						/**< BC2 (DXT2-DXT3) compression */
	STUDIOAPI_PIXEL_FORMAT_BC3,						/**< BC3 (DXT4-DXT5) compression */
	STUDIOAPI_PIXEL_FORMAT_BC4,						/**< BC4 (ATI1) compression */
	STUDIOAPI_PIXEL_FORMAT_BC5,						/**< BC5 (ATI2) compression */
	STUDIOAPI_PIXEL_FORMAT_BC6H,					/**< BC6 compression */
	STUDIOAPI_PIXEL_FORMAT_BC7,						/**< BC7 compression */

	// Other formats
	STUDIOAPI_PIXEL_FORMAT_DEPTH16,					/**< Depth 16 bit */
	STUDIOAPI_PIXEL_FORMAT_DEPTH32F,				/**< Depth 32 bit float */
	STUDIOAPI_PIXEL_FORMAT_DEPTH16_STENCIL8,		/**< Depth 16 bit and stencil 8 bit */
	STUDIOAPI_PIXEL_FORMAT_DEPTH24_STENCIL8,		/**< Depth 24 bit and stencil 8 bit */
	STUDIOAPI_PIXEL_FORMAT_DEPTH32F_STENCIL8,		/**< Depth 32 bit float and stencil 8 bit */
	STUDIOAPI_PIXEL_NUM_FORMATS						/**< Count pixel formats */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI sampler address mode
 */
enum studioAPISamplerAddressMode_t
{
	STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP,				/**< Wrap */
	STUDIOAPI_SAMPLER_ADDRESS_MODE_CLAMP,				/**< Clamp */
	STUDIOAPI_SAMPLER_ADDRESS_MODE_MIRROR_WRAP,			/**< Mirror wrap */
	STUDIOAPI_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP,		/**< Mirror clamp */
	STUDIOAPI_SAMPLER_ADDRESS_MODE_BORDER,				/**< Border */
	STUDIOAPI_SAMPLER_ADDRESS_NUM_MODES					/**< Count sampler address modes */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI sampler filter
 */
enum studioAPISamplerFilter_t
{
	STUDIOAPI_SAMPLER_FILTER_POINT,			/**< No filter */
	STUDIOAPI_SAMPLER_FILTER_BILINEAR,		/**< Bilinear */
	STUDIOAPI_SAMPLER_FILTER_TRILINEAR,		/**< Trilinear */
	STUDIOAPI_SAMPLER_FILTER_ANISOTROPIC,	/**< Anisotropic */
	STUDIOAPI_SAMPLER_NUM_FILTERS			/**< Count sampler filters */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI sampler compare operation
 */
enum studioAPISamplerCompareOp_t
{
	STUDIOAPI_SAMPLER_COMPARE_OP_NEVER,		/**< Never */
	STUDIOAPI_SAMPLER_COMPARE_OP_LESS,		/**< Less */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI texture type
 */
enum studioAPITextureType_t
{
	STUDIOAPI_TEXTURE_TYPE_1D,			/**< 1D texture */
	STUDIOAPI_TEXTURE_TYPE_2D,			/**< 2D texture */
	STUDIOAPI_TEXTURE_TYPE_CUBE,		/**< Cube texture */
	STUDIOAPI_TEXTURE_TYPE_3D,			/**< 3D texture */
	STUDIOAPI_TEXTURE_TYPE_1D_ARRAY,	/**< 1D array texture */
	STUDIOAPI_TEXTURE_TYPE_2D_ARRAY,	/**< 2D array texture */
	STUDIOAPI_TEXTURE_TYPE_CUBE_ARRAY,	/**< Cube array texture */
	STUDIOAPI_TEXTURE_NUM_TYPES			/**< Texture types number */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI texture memory flags
 */
enum studioAPITextureMemoryFlag_t
{
	STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU		= 1 << 0,																		/**< CPU memory */
	STUDIOAPI_TEXTURE_MEMORY_FLAG_GPU		= 1 << 1,																		/**< GPU memory */
	STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU_GPU	= STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU | STUDIOAPI_TEXTURE_MEMORY_FLAG_GPU			/**< Shared memory between CPU and GPU */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI texture usage flags
 */
enum studioAPITextureUsageFlag_t
{
	STUDIOAPI_TEXTURE_USAGE_FLAG_CONCURRENT			= 1 << 0,	/**< The texture will be often used in a few queue families */
	STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_SRC		= 1 << 1,	/**< The texture will be used as transfer source */
	STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST		= 1 << 2,	/**< The texture will be used as transfer destination */
	STUDIOAPI_TEXTURE_USAGE_FLAG_TEXTURE			= 1 << 3,	/**< The texture will be used as texture in shaders */
	STUDIOAPI_TEXTURE_USAGE_FLAG_RENDER_TARGET		= 1 << 4,	/**< The texture will be used as render target in frame buffers */
	STUDIOAPI_TEXTURE_USAGE_FLAG_UNORDERED_ACCESS	= 1 << 5,	/**< The texture will be used as unordered access in shaders */
	STUDIOAPI_TEXTURE_USAGE_FLAG_LINEAR				= 1 << 6	/**< The texture data will be laid out in memory in row-major order, possibly with some padding on each row. Limits: mips and layers number must be equal to 1 */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI sampler create information
 */
struct studioAPISamplerCreateInfo_t
{
	studioAPISamplerFilter_t		filer;				/**< Filter */
	studioAPISamplerAddressMode_t	addressModeU;		/**< Address mode for U coord */
	studioAPISamplerAddressMode_t	addressModeV;		/**< Address mode for V coord */
	studioAPISamplerAddressMode_t	addressModeW;		/**< Address mode for W coord */
	float							mipBias;			/**< Bias to be added to mipmap LOD calculation */
	uint32							maxAnisotropy;		/**< Is the anisotropy value clamp used by the sampler when bAnisotropyEnable is TRUE. If bAnisotropyEnable is FALSE, maxAnisotropy is ignored */
	studioAPISamplerCompareOp_t		compareOp;			/**< Comparison operator to apply to fetched data before filtering. If the compareOp is STUDIOAPI_SAMPLER_COMPARE_OP_NEVER will be ignored */
	float							minLod;				/**< Is used to clamp the minimum of the computed LOD value */
	float							maxLod;				/**< Is used to clamp the maximum of the computed LOD value. To avoid clamping the maximum value, set maxLod to max float value (i.g FLT_MAX) */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI mapped a texture data
 */
struct studioAPIMappedTextureData_t
{
	byte*		pData;		/**< Data */
	uint64		size;		/**< Data size */
	uint64		rowPitch;	/**< Data row pitch */
	uint64		depthPitch;	/**< Data depth pitch */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI sampler interface
 */
class IStudioAPISampler : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPISampler() {}
};

/**
 * @ingroup studioapi
 * @brief StudioAPI texture interface
 */
class IStudioAPITexture : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPITexture() {}

	/**
	 * @brief Update a data in the texture
	 * @warning For use the texture must have STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST flag
	 *
	 * @param pCmdContext	Command context
	 * @param pData			A new texture data
	 * @param startLayer	Start layer index
	 * @param startMip		Start mip index
	 */
	virtual void UpdateData( IStudioAPICmdContext* pCmdContext, byte* pData, uint32 startLayer = 0, uint32 startMip = 0 ) = 0;

	/**
	 * @brief Map memory of the texture data
	 * @warning Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU is set then you can use the function,
	 * otherwise use IStudioAPICmdList::CopyBufferToTexture, IStudioAPICmdList::CopyTextureToBuffer or IStudioAPICmdList::CopyTexture
	 *
	 * @param layer			Layer index
	 * @param mip			Mip level
	 * @param mappedData	Output mapped data
	 */
	virtual void MapMemory( uint32 layer, uint32 mip, studioAPIMappedTextureData_t& mappedData ) = 0;

	/**
	 * @brief Unmap memory of the texture data
	 * @warning Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU is set then you can use the function,
	 * otherwise use IStudioAPICmdList::CopyBufferToTexture, IStudioAPICmdList::CopyTextureToBuffer or IStudioAPICmdList::CopyTexture
	 *
	 * @param mappedData	Mapped data to unmap
	 */
	virtual void UnmapMemory( studioAPIMappedTextureData_t& mappedData ) = 0;

	/**
	 * @brief Get texture width
	 * @return Return texture width
	 */
	virtual uint32 GetSizeX() const = 0;

	/**
	 * @brief Get texture height
	 * @return Return texture height
	 */
	virtual uint32 GetSizeY() const = 0;

	/**
	 * @brief Get texture depth
	 * @return Return texture depth
	 */
	virtual uint32 GetSizeZ() const = 0;

	/**
	 * @brief Get mip levels number in the texture
	 * @return Return mip levels number in the texture
	 */
	virtual uint32 GetNumMips() const = 0;

	/**
	 * @brief Get layers number in the texture
	 * @return Return layers number in the texture
	 */
	virtual uint32 GetNumLayers() const = 0;

	/**
	 * @brief Get pixel format of the texture
	 * @return Return pixel format of the texture
	 */
	virtual studioAPIPixelFormat_t GetPixelFormat() const = 0;

	/**
	 * @brief Get texture type
	 * @return Return texture type
	 */
	virtual studioAPITextureType_t GetType() const = 0;

	/**
	 * @brief Get usage flags
	 * @return Return usage flags (see studioAPITextureUsageFlag_t)
	 */
	virtual uint32 GetUsageFlags() const = 0;

	/**
	 * @brief Get texture memory flags
	 * @return Return texture memory flags (see studioAPITextureMemoryFlag_t)
	 */
	virtual uint8 GetMemoryFlags() const = 0;
};

#endif // !ISTUDIOAPI_TEXTURE_H