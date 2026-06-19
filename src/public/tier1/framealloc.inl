#pragma once

/*
==================
CFrameAlloc::CFrameAlloc
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::CFrameAlloc( const char* pAllocName /* = "Unknown" */ )
	: currentPoolId( INVALID_INDEX )
	, pAllocName( pAllocName )
{
	// Allocate one page for each pool
	for ( uint32 index = 0; index < numPools; ++index )
	{
		memoryPool_t& pool = pools[index];
		pool.id			   = index;
		pool.bIsFree.store( true, eastl::memory_order_relaxed );
		AllocBlock( pool );
	}
}

/*
==================
CFrameAlloc::~CFrameAlloc
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::~CFrameAlloc()
{
	// Free all pages
	for ( uint32 index = 0; index < numPools; ++index )
	{
		FreeAllBlocks( pools[index] );
	}
}

/*
==================
CFrameAlloc::Alloc
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE void* CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::Alloc( size numBytes, uint32 alignment /* = 0 */ )
{
	return Alloc( numBytes, alignment, NULL );
}

/*
==================
CFrameAlloc::Alloc
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
void* CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::Alloc( size numBytes, uint32 alignment, memoryBlock_t** pBlock )
{
	// Calculate alignment and aligned size
	PROFILER_SCOPE_FUNC();
	Assert( currentPoolId != INVALID_INDEX );
	alignment				   = S_Max<uint32>( numBytes >= defaultAlignment ? defaultAlignment : minAlignment, alignment );
	byte*		   pPtr		   = NULL;
	memoryBlock_t* pFoundBlock = NULL;
	memoryPool_t&  pool		   = pools[currentPoolId];
	size		   alignedSize = S_Align<size>( numBytes, alignment );
	AssertMsg( alignedSize <= blockSize, "A frame allocator can't allocate memory more then block size" );

	// Try to allocate memory in already created blocks
	for ( auto it = pool.blockList.begin(), itEnd = pool.blockList.end(); it != itEnd; ++it )
	{
		memoryBlock_t& block	= *it;
		uint64		   freeSize = block.allocatedSize - block.usedSize;
		if ( freeSize < alignedSize )
		{
			continue;
		}

		byte*  pFreeMemory		  = block.pData + block.usedSize;
		byte*  pAlignedFreeMemory = S_Align<byte*>( pFreeMemory, alignment );
		uint64 alignPadding		  = (uint64)( pAlignedFreeMemory - pFreeMemory );
		if ( freeSize < ( alignedSize + alignPadding ) )
		{
			continue;
		}

		pPtr		= pAlignedFreeMemory;
		pFoundBlock = &block;
		block.usedSize += alignedSize + alignPadding;
		break;
	}

	// Allocate a new block if we not found a space yet
	if ( !pPtr )
	{
		AllocBlock( pool );

		memoryBlock_t& block			  = pool.blockList.back();
		byte*		   pAlignedFreeMemory = S_Align<byte*>( block.pData, alignment );
		uint64		   alignPadding		  = (uint64)( pAlignedFreeMemory - block.pData );
		Assert( block.allocatedSize >= alignedSize + alignPadding );

		pPtr		= pAlignedFreeMemory;
		pFoundBlock = &block;
		block.usedSize += alignedSize + alignPadding;
	}

	if ( pBlock )
	{
		*pBlock = pFoundBlock;
	}
	return pPtr;
}

/*
==================
CFrameAlloc::Construct
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
template<typename TType, typename... TArgs>
FORCEINLINE TType* CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::Construct( TArgs&&... args )
{
	PROFILER_SCOPE_FUNC();
	memoryBlock_t* pBlock  = NULL;
	void*		   pPtr	   = Alloc( sizeof( TType ), alignof( TType ), &pBlock );
	TType*		   pObject = new ( pPtr ) TType( eastl::forward<TArgs>( args )... );
	if constexpr ( !eastl::is_trivially_destructible_v<TType> )
	{
		destructorEntry_t destructorEntry;
		destructorEntry.pObject			 = pObject;
		destructorEntry.pDestroyObjectFn = []( void* pObject )
		{
			( (TType*)pObject )->~TType();
		};

		Assert( pBlock );
		pBlock->destructorList.emplace_back( destructorEntry );
	}

	return pObject;
}

/*
==================
CFrameAlloc::SwapPools
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
void CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::SwapPools()
{
	// Wait a free pool
	PROFILER_SCOPE_FUNC();
	WaitFreePool();

	// Mark the pool as used
	pools[currentPoolId].bIsFree.store( false, eastl::memory_order_relaxed );
}

/*
==================
CFrameAlloc::MarkAsFreePool
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE void CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::MarkAsFreePool( uint32 index )
{
	// Reset used size and destruct objects in blocks
	memoryPool_t& pool = pools[index];
	for ( auto it = pool.blockList.begin(), itEnd = pool.blockList.end(); it != itEnd; ++it )
	{
		memoryBlock_t& curBlock = *it;
		for ( auto itDestructorEntry = curBlock.destructorList.begin(), itDestructorEntryEnd = curBlock.destructorList.end(); itDestructorEntry != itDestructorEntryEnd; ++itDestructorEntry )
		{
			destructorEntry_t& destructorEntry = *itDestructorEntry;
			destructorEntry.pDestroyObjectFn( destructorEntry.pObject );
		}

		curBlock.usedSize = 0;
		curBlock.destructorList.clear();
	}

	// Mark the pool as free
	pools[index].bIsFree.store( true, eastl::memory_order_release );
}

/*
==================
CFrameAlloc::GetCurrentPoolId
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE uint32 CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::GetCurrentPoolId() const
{
	return currentPoolId;
}

/*
==================
CFrameAlloc::AllocBlock
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE void CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::AllocBlock( memoryPool_t& pool )
{
	memoryBlock_t& block = pool.blockList.emplace_back();
	block.allocatedSize	 = blockSize;
	block.usedSize		 = 0;
	block.pData			 = (byte*)Mem_MallocZero( block.allocatedSize );
	Msg( "FrameAlloc: Allocated block (%llu bytes, pool: %i, name: '%s')", block.allocatedSize, pool.id, pAllocName );
	PROFILER_MEM_ALLOC( block.pData, block.allocatedSize, pAllocName );
}

/*
==================
CFrameAlloc::FreeBlock
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
void CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::FreeBlock( memoryPool_t& pool, memoryBlock_t* pBlock )
{
	PROFILER_SCOPE_FUNC();
	Assert( pBlock );
	for ( auto it = pool.blockList.begin(), itEnd = pool.blockList.end(); it != itEnd; ++it )
	{
		memoryBlock_t& curBlock = *it;
		if ( &curBlock == pBlock )
		{
			for ( auto itDestructorEntry = curBlock.destructorList.begin(), itDestructorEntryEnd = curBlock.destructorList.end(); itDestructorEntry != itDestructorEntryEnd; ++itDestructorEntry )
			{
				destructorEntry_t& destructorEntry = *itDestructorEntry;
				destructorEntry.pDestroyObjectFn( destructorEntry.pObject );
			}

			if ( curBlock.pData )
			{
				Mem_Free( curBlock.pData );
				PROFILER_MEM_FREE( curBlock.pData, pAllocName );
			}

			Msg( "FrameAlloc: Freed block (%llu bytes, pool: %i, name: '%s')", curBlock.allocatedSize, pool.id, pAllocName );
			pool.blockList.erase( it );
			break;
		}
	}
}

/*
==================
CFrameAlloc::FreeAllBlocks
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
void CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::FreeAllBlocks( memoryPool_t& pool )
{
	// Free blocks
	PROFILER_SCOPE_FUNC();
	uint64 totalUsedMemory = 0;
	uint64 numBlocks	   = pool.blockList.size();
	for ( auto it = pool.blockList.begin(), itEnd = pool.blockList.end(); it != itEnd; ++it )
	{
		memoryBlock_t& curBlock = *it;
		totalUsedMemory += curBlock.allocatedSize;

		for ( auto itDestructorEntry = curBlock.destructorList.begin(), itDestructorEntryEnd = curBlock.destructorList.end(); itDestructorEntry != itDestructorEntryEnd; ++itDestructorEntry )
		{
			destructorEntry_t& destructorEntry = *itDestructorEntry;
			destructorEntry.pDestroyObjectFn( destructorEntry.pObject );
		}

		if ( curBlock.pData )
		{
			Mem_Free( curBlock.pData );
			PROFILER_MEM_FREE( curBlock.pData, pAllocName );
		}
	}

	pool.blockList.clear();
	Msg( "FrameAlloc: Freed %i blocks (%llu bytes, pool: %i, name: '%s')", numBlocks, totalUsedMemory, pool.id, pAllocName );
}

/*
==================
CFrameAlloc::WaitFreePool
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE void CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::WaitFreePool()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_WAIT );
	while ( !pools[currentPoolId = ( currentPoolId + 1 ) % numPools].bIsFree.load( eastl::memory_order_acquire ) )
	{
		Sys_Yield();
	}
}
