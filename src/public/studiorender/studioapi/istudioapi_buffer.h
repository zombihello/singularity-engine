#pragma once
#include "tier0/types.h"
#include "studiorender/studioapi/istudioapi_resource.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdList;

//-----------------------------------------------------------------------------
// Studio API buffer flags and states
//-----------------------------------------------------------------------------
enum studioAPIBufferUsageFlag_t
{
	STUDIOAPI_BUFFER_USAGE_FLAG_STATIC			  = 1 << 0,																		// The buffer will be created, filled and never repacked
	STUDIOAPI_BUFFER_USAGE_FLAG_DYNAMIC			  = 1 << 1,																		// The buffer will be repacked in-frequently
	STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE		  = 1 << 2,																		// The buffer will be repacked every frame or every draw, its contents valid only until the end of the current frame
	STUDIOAPI_BUFFER_USAGE_FLAG_CONCURRENT		  = 1 << 3,																		// The buffer will be often used in a few queue families
	STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_SRC	  = 1 << 4,																		// The buffer will be used as transfer source
	STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST	  = 1 << 5,																		// The buffer will be used as transfer destination
	STUDIOAPI_BUFFER_USAGE_FLAG_VERTEX_BUFFER	  = 1 << 6,																		// The buffer is vertex buffer
	STUDIOAPI_BUFFER_USAGE_FLAG_INDEX_BUFFER	  = 1 << 7,																		// The buffer is index buffer
	STUDIOAPI_BUFFER_USAGE_FLAG_CONSTANT_BUFFER	  = 1 << 8,																		// The buffer is constant buffer
	STUDIOAPI_BUFFER_USAGE_FLAG_STRUCTURED_BUFFER = 1 << 9,																		// The buffer is structured buffer
	STUDIOAPI_BUFFER_USAGE_FLAG_ANY_DYNAMIC		  = STUDIOAPI_BUFFER_USAGE_FLAG_DYNAMIC | STUDIOAPI_BUFFER_USAGE_FLAG_VOLATILE	// Helper bit-mask for dynamic
};

enum studioAPIBufferMemoryFlag_t
{
	STUDIOAPI_BUFFER_MEMORY_FLAG_CPU	 = 1 << 0,
	STUDIOAPI_BUFFER_MEMORY_FLAG_GPU	 = 1 << 1,
	STUDIOAPI_BUFFER_MEMORY_FLAG_CPU_GPU = STUDIOAPI_BUFFER_MEMORY_FLAG_CPU | STUDIOAPI_BUFFER_MEMORY_FLAG_GPU
};

enum studioAPIBufferState_t
{
	STUDIOAPI_BUFFER_STATE_UNDEFINED				= 0,	   // Invalid state (don't preserve contents)
	STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE			= 1 << 0,  // Shader resource, read and write
	STUDIOAPI_BUFFER_STATE_SHADER_RESOURCE_READONLY = 1 << 1,  // Shader resource, read only
	STUDIOAPI_BUFFER_STATE_TRANSFER_SRC				= 1 << 2,  // Copy a data from
	STUDIOAPI_BUFFER_STATE_TRANSFER_DST				= 1 << 3,  // Copy a data to
	STUDIOAPI_BUFFER_STATE_VERTEX_BUFFER			= 1 << 4,  // Vertex buffer, read only
	STUDIOAPI_BUFFER_STATE_INDEX_BUFFER				= 1 << 5,  // Index buffer, read only
	STUDIOAPI_BUFFER_STATE_CONSTANT_BUFFER			= 1 << 6,  // Constant buffer, read only
	STUDIOAPI_BUFFER_STATE_STRUCTURED_BUFFER		= 1 << 7   // Structured buffer, read only
};

struct studioAPIMappedBufferData_t
{
	byte*  pData; /**< Data */
	uint64 size;  /**< Data size */
};

//-----------------------------------------------------------------------------
// StudioAPI buffer interface
//-----------------------------------------------------------------------------
class IStudioAPIBuffer : public IStudioAPIResource
{
public:
	virtual ~IStudioAPIBuffer() {}

	virtual uint32 GetUsageFlags() const  = 0;
	virtual uint64 GetSize() const		  = 0;
	virtual uint32 GetStride() const	  = 0;
	virtual uint8  GetMemoryFlags() const = 0;

	// NOTE: Before use check memory flags by GetMemoryFlags(). If STUDIOAPI_BUFFER_MEMORY_FLAG_CPU is set then you can use the function,
	// otherwise use IStudioAPICmdList::CopyBuffer
	virtual void MapMemory( uint64 size, uint64 offset, studioAPIMappedBufferData_t& mappedData ) = 0;
	virtual void UnmapMemory( studioAPIMappedBufferData_t& mappedData )							  = 0;

	// For use the buffer must have STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST flag
	virtual void UpdateData( IStudioAPICmdContext* pCmdContext, byte* pData, uint64 dataSize, uint64 offset = 0 ) = 0;
};
