/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_BUFFER_H
#define ISTUDIOAPI_BUFFER_H
 
#include "core/types.h"
#include "studiorender/studioapi/istudioapi_resource.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"

// Forward declarations
class IStudioAPICmdList;

/**
 * @ingroup studioapi
 * @brief StudioAPI buffer usage flags
 */
enum studioAPIBufferUsageFlag_t
{
	STUDIOAPI_BUFFER_USAGE_FLAG_STATIC				= 1 << 0,																			/**< The buffer will be created, filled and never repacked */
	STUDIOAPI_BUFFER_USAGE_FLAG_DYNAMIC				= 1 << 1,																			/**< The buffer will be repacked in-frequently */
	STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE			= 1 << 2,																			/**< The buffer will be repacked every frame */
	STUDIOAPI_BUFFER_USAGE_FLAG_CONCURRENT			= 1 << 3,																			/**< The buffer will be often used in a few queue families */
	STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_SRC		= 1 << 4,																			/**< The buffer will be used as transfer source */
	STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST		= 1 << 5,																			/**< The buffer will be used as transfer destination */
	STUDIOAPI_BUFFER_USAGE_FLAG_VERTEX_BUFFER		= 1 << 6,																			/**< The buffer is vertex buffer */
	STUDIOAPI_BUFFER_USAGE_FLAG_INDEX_BUFFER		= 1 << 7,																			/**< The buffer is index buffer */
	STUDIOAPI_BUFFER_USAGE_FLAG_CONSTANT_BUFFER		= 1 << 8,																			/**< The buffer is constant buffer */
	STUDIOAPI_BUFFER_USAGE_FLAG_STRUCTURED_BUFFER	= 1 << 9,																			/**< The buffer is structured buffer */
	STUDIOAPI_BUFFER_USAGE_FLAG_ANY_DYNAMIC			= STUDIOAPI_BUFFER_USAGE_FLAG_DYNAMIC | STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE		/**< Helper bit-mask for dynamic */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI buffer memory flags
 */
enum studioAPIBufferMemoryFlag_t
{
	STUDIOAPI_BUFFER_MEMORY_FLAG_CPU				= 1 << 0,																		/**< CPU memory */
	STUDIOAPI_BUFFER_MEMORY_FLAG_GPU				= 1 << 1,																		/**< GPU memory */
	STUDIOAPI_BUFFER_MEMORY_FLAG_CPU_GPU			= STUDIOAPI_BUFFER_MEMORY_FLAG_CPU | STUDIOAPI_BUFFER_MEMORY_FLAG_GPU			/**< Shared memory between CPU and GPU */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI buffer state
 */
enum studioAPIBufferState_t
{
	STUDIOAPI_BUFFER_STATE_UNDEFINED				= 0,					/**< Invalid state (don't preserve contents) */
	STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE			= 1 << 0,				/**< Shader resource, read and write */
	STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE_READONLY	= 1 << 1,				/**< Shader resource, read only */
	STUDIOAPI_BUFFER_STATE_TRANSFER_SRC				= 1 << 2,				/**< Copy a data from */
	STUDIOAPI_BUFFER_STATE_TRANSFER_DST				= 1 << 3,				/**< Copy a data to */
	STUDIOAPI_BUFFER_STATE_VERTEX_BUFFER			= 1 << 4,				/**< Vertex buffer, read only */
	STUDIOAPI_BUFFER_STATE_INDEX_BUFFER				= 1 << 5,				/**< Index buffer, read only */
	STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER			= 1 << 6,				/**< Constant buffer, read only */
	STUDIOAPI_BUFFER_STATE_STRUCTURED_BUFFER		= 1 << 7				/**< Structured buffer, read only */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI mapped a buffer data
 */
struct studioAPIMappedBufferData_t
{
	byte*		pData;	/**< Data */
	uint64		size;	/**< Data size */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI buffer interface
 */
class IStudioAPIBuffer : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPIBuffer() {}

	/**
	 * @brief Get usage flags
	 * @return Return usage flags (see studioAPIBufferUsageFlag_t)
	 */
	virtual uint32 GetUsageFlags() const = 0;

	/**
	 * @brief Get buffer size
	 * @return Return buffer size
	 */
	virtual uint64 GetSize() const = 0;

	/**
	 * @brief Get buffer stride
	 * @return Return buffer stride
	 */
	virtual uint32 GetStride() const = 0;

	/**
	 * @brief Get buffer memory flags
	 * @return Return buffer memory flags (see studioAPIBufferMemoryFlag_t)
	 */
	virtual uint8 GetMemoryFlags() const = 0;

	/**
	 * @brief Map memory of the buffer data
	 * @warning Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_BUFFER_MEMORY_FLAG_CPU is set then you can use the function,
	 * otherwise use IStudioAPICmdList::CopyBuffer
	 * 
	 * @param size			Data size to map
	 * @param offset		Offset in the buffer
	 * @param mappedData	Output mapped data
	 */
	virtual void MapMemory( uint64 size, uint64 offset, studioAPIMappedBufferData_t& mappedData ) = 0;

	/**
	 * @brief Unmap memory of the buffer data
	 * @warning Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_BUFFER_MEMORY_FLAG_CPU is set then you can use the function,
	 * otherwise use IStudioAPICmdList::CopyBuffer
	 * 
	 * @param mappedData	Mapped data to unmap
	 */
	virtual void UnmapMemory( studioAPIMappedBufferData_t& mappedData ) = 0;

	/**
	 * @brief Update a data in the buffer
	 * @warning For use the buffer must have STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST flag 
	 * 
	 * @param pCmdContext	Command context
	 * @param pData			A new buffer data
	 * @param dataSize		Data size
	 * @param offset		Buffer offset
	 */
	virtual void UpdateData( IStudioAPICmdContext* pCmdContext, byte* pData, uint64 dataSize, uint64 offset = 0 ) = 0;
};

#endif // !ISTUDIOAPI_BUFFER_H