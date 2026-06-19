#pragma once
#include <EASTL/atomic.h>
#include <EASTL/list.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier0/iprofiler.h"

//-----------------------------------------------------------------------------
// Frame memory allocator
// The allocator used to allocate memory for a frame data.
// All temporary data, like dynamic tesselations and local spaces are allocated by the one
//
// NOTES:
//	* Frame allocator hasn't any free ranges, after each allocation a pointer move forward to free position
//	* On beginning of a frame the allocator resets and whole the memory will be reused
//	* Based on point 1 and 2, the allocator not supported Free and Realloc functions
//	* Before use the allocator you must call CFrameAlloc::SwapPools to get a pool to use
//-----------------------------------------------------------------------------
template<uint32 blockSize = 64 * 1024 * 1024, uint32 numPools = 1, uint32 defaultAlignment = 16, uint32 minAlignment = 8>
class CFrameAlloc
{
public:
	CFrameAlloc( const char* pAllocName = "Unknown" );
	~CFrameAlloc();

	template<typename TType, typename... TArgs>
	TType* Construct( TArgs&&... args );
	void*  Alloc( size numBytes, uint32 alignment = 0 );
	void   SwapPools();
	void   MarkAsFreePool( uint32 index );
	uint32 GetCurrentPoolId() const;

private:
	struct destructorEntry_t
	{
		void ( *pDestroyObjectFn )( void* pObject );
		void* pObject;
	};

	struct memoryBlock_t
	{
		uint64						   allocatedSize;
		uint64						   usedSize;
		byte*						   pData;
		eastl::list<destructorEntry_t> destructorList;
	};

	struct memoryPool_t
	{
		uint32					   id;
		eastl::atomic<bool>		   bIsFree;
		eastl::list<memoryBlock_t> blockList;
	};

	void* Alloc( size numBytes, uint32 alignment, memoryBlock_t** pBlock );
	void  AllocBlock( memoryPool_t& pool );
	void  FreeBlock( memoryPool_t& pool, memoryBlock_t* pBlock );
	void  FreeAllBlocks( memoryPool_t& pool );
	void  WaitFreePool();

	uint32		 currentPoolId;
	memoryPool_t pools[numPools];
	const char*	 pAllocName;
};

#include "tier1/framealloc.inl"
