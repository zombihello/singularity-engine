#pragma once
#include "studiorender/studioapi/istudioapi_resource.h"
#include "studiorender/studioapi/istudioapi_renderpipeline.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdContext;


//-----------------------------------------------------------------------------
// Texture enums
//-----------------------------------------------------------------------------
enum studioAPITextureLayout_t
{
	STUDIOAPI_TEXTURE_LAYOUT_UNDEFINED,
	STUDIOAPI_TEXTURE_LAYOUT_SHADER_RESOURCE,
	STUDIOAPI_TEXTURE_LAYOUT_SHADER_RESOURCE_READONLY,
	STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_SRC,
	STUDIOAPI_TEXTURE_LAYOUT_TRANSFER_DST,
	STUDIOAPI_TEXTURE_LAYOUT_COLOR_RENDER_TARGET,
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_STENCIL_RENDER_TARGET,
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_READONLY_STENCIL_RENDER_TARGET,
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_RENDER_TARGET_STENCIL_READONLY,
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_READONLY_STENCIL_READONLY,		
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_RENDER_TARGET,					
	STUDIOAPI_TEXTURE_LAYOUT_DEPTH_READONLY,						
	STUDIOAPI_TEXTURE_LAYOUT_STENCIL_RENDER_TARGET,					
	STUDIOAPI_TEXTURE_LAYOUT_STENCIL_READONLY,						
	STUDIOAPI_TEXTURE_LAYOUT_PRESENT								
};


enum studioAPIPixelFormat_t
{
	STUDIOAPI_PIXEL_FORMAT_UNKNOWN,

	// Channel component formats
	STUDIOAPI_PIXEL_FORMAT_RGBA8,
	STUDIOAPI_PIXEL_FORMAT_RG8,
	STUDIOAPI_PIXEL_FORMAT_R8,
	STUDIOAPI_PIXEL_FORMAT_RGBA16,
	STUDIOAPI_PIXEL_FORMAT_RG16,
	STUDIOAPI_PIXEL_FORMAT_R16,
	STUDIOAPI_PIXEL_FORMAT_RGBA32F,
	STUDIOAPI_PIXEL_FORMAT_RG32F,
	STUDIOAPI_PIXEL_FORMAT_R32F,
	STUDIOAPI_PIXEL_FORMAT_RGBA16F,
	STUDIOAPI_PIXEL_FORMAT_RG16F,
	STUDIOAPI_PIXEL_FORMAT_R16F,

	// Compression formats
	STUDIOAPI_PIXEL_FORMAT_BC1,
	STUDIOAPI_PIXEL_FORMAT_BC2,
	STUDIOAPI_PIXEL_FORMAT_BC3,
	STUDIOAPI_PIXEL_FORMAT_BC4,
	STUDIOAPI_PIXEL_FORMAT_BC5,
	STUDIOAPI_PIXEL_FORMAT_BC6H,
	STUDIOAPI_PIXEL_FORMAT_BC7,

	// Other formats
	STUDIOAPI_PIXEL_FORMAT_DEPTH16,
	STUDIOAPI_PIXEL_FORMAT_DEPTH32F,
	STUDIOAPI_PIXEL_FORMAT_DEPTH16_STENCIL8,
	STUDIOAPI_PIXEL_FORMAT_DEPTH24_STENCIL8,
	STUDIOAPI_PIXEL_FORMAT_DEPTH32F_STENCIL8,
	STUDIOAPI_PIXEL_NUM_FORMATS
};


enum studioAPISamplerAddressMode_t
{
	STUDIOAPI_SAMPLER_ADDRESS_MODE_WRAP,
	STUDIOAPI_SAMPLER_ADDRESS_MODE_CLAMP,
	STUDIOAPI_SAMPLER_ADDRESS_MODE_MIRROR_WRAP,
	STUDIOAPI_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP,
	STUDIOAPI_SAMPLER_ADDRESS_MODE_BORDER,
	STUDIOAPI_SAMPLER_ADDRESS_NUM_MODES
};


enum studioAPISamplerFilter_t
{
	STUDIOAPI_SAMPLER_FILTER_POINT,			
	STUDIOAPI_SAMPLER_FILTER_BILINEAR,		
	STUDIOAPI_SAMPLER_FILTER_TRILINEAR,		
	STUDIOAPI_SAMPLER_FILTER_ANISOTROPIC,	
	STUDIOAPI_SAMPLER_NUM_FILTERS			
};


enum studioAPISamplerCompareOp_t
{
	STUDIOAPI_SAMPLER_COMPARE_OP_NEVER,
	STUDIOAPI_SAMPLER_COMPARE_OP_LESS,
};


enum studioAPITextureType_t
{
	STUDIOAPI_TEXTURE_TYPE_1D,
	STUDIOAPI_TEXTURE_TYPE_2D,
	STUDIOAPI_TEXTURE_TYPE_CUBE,
	STUDIOAPI_TEXTURE_TYPE_3D,
	STUDIOAPI_TEXTURE_TYPE_1D_ARRAY,
	STUDIOAPI_TEXTURE_TYPE_2D_ARRAY,
	STUDIOAPI_TEXTURE_TYPE_CUBE_ARRAY,
	STUDIOAPI_TEXTURE_NUM_TYPES
};


enum studioAPITextureMemoryFlag_t
{
	STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU		= 1 << 0,																		
	STUDIOAPI_TEXTURE_MEMORY_FLAG_GPU		= 1 << 1,																		
	STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU_GPU	= STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU | STUDIOAPI_TEXTURE_MEMORY_FLAG_GPU			
};


enum studioAPITextureUsageFlag_t
{
	STUDIOAPI_TEXTURE_USAGE_FLAG_CONCURRENT			= 1 << 0,	
	STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_SRC		= 1 << 1,	
	STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST		= 1 << 2,	
	STUDIOAPI_TEXTURE_USAGE_FLAG_TEXTURE			= 1 << 3,	
	STUDIOAPI_TEXTURE_USAGE_FLAG_RENDER_TARGET		= 1 << 4,	
	STUDIOAPI_TEXTURE_USAGE_FLAG_UNORDERED_ACCESS	= 1 << 5,	
	STUDIOAPI_TEXTURE_USAGE_FLAG_LINEAR				= 1 << 6	
};


//-----------------------------------------------------------------------------
// Texture structs
//-----------------------------------------------------------------------------
struct studioAPISamplerCreateInfo_t
{
	studioAPISamplerFilter_t		filer;				
	studioAPISamplerAddressMode_t	addressModeU;		
	studioAPISamplerAddressMode_t	addressModeV;		
	studioAPISamplerAddressMode_t	addressModeW;		
	float							mipBias;			
	uint32							maxAnisotropy;
	studioAPISamplerCompareOp_t		compareOp;			// If the compareOp is STUDIOAPI_SAMPLER_COMPARE_OP_NEVER will be ignored
	float							minLod;
	float							maxLod;				// To avoid clamping the maximum value, set maxLod to max float value (i.g FLT_MAX)
};


struct studioAPIMappedTextureData_t
{
	byte*		pData;
	uint64		size;
	uint64		rowPitch;
	uint64		depthPitch;	
};


//-----------------------------------------------------------------------------
// StudioAPI sampler interface
//-----------------------------------------------------------------------------
class IStudioAPISampler : public IStudioAPIResource
{
public:
	virtual ~IStudioAPISampler() {}
};


//-----------------------------------------------------------------------------
// StudioAPI texture interface
//-----------------------------------------------------------------------------
class IStudioAPITexture : public IStudioAPIResource
{
public:
	virtual ~IStudioAPITexture() {}

	// For use the texture must have STUDIOAPI_TEXTURE_USAGE_FLAG_TRANSFER_DST flag
	virtual void UpdateData( IStudioAPICmdContext* pCmdContext, byte* pData, uint32 startLayer = 0, uint32 startMip = 0 ) = 0;

	// NOTE: Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_TEXTURE_MEMORY_FLAG_CPU is set then you can use the function,
	// otherwise use IStudioAPICmdList::CopyBufferToTexture, IStudioAPICmdList::CopyTextureToBuffer or IStudioAPICmdList::CopyTexture
	virtual void MapMemory( uint32 layer, uint32 mip, studioAPIMappedTextureData_t& mappedData ) = 0;
	virtual void UnmapMemory( studioAPIMappedTextureData_t& mappedData ) = 0;

	virtual uint32 GetSizeX() const = 0;
	virtual uint32 GetSizeY() const = 0;
	virtual uint32 GetSizeZ() const = 0;
	virtual uint32 GetNumMips() const = 0;
	virtual uint32 GetNumLayers() const = 0;
	virtual studioAPIPixelFormat_t GetPixelFormat() const = 0;
	virtual studioAPITextureType_t GetType() const = 0;
	virtual uint32 GetUsageFlags() const = 0;
	virtual uint8 GetMemoryFlags() const = 0;
};