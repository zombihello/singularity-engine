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
		memoryPool_t& pool	= pools[index];
		pool.id				= index;
		pool.currentBlockIt = AllocBlock( pool );
		pool.bIsFree.store( true, eastl::memory_order_relaxed );
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
CFrameAlloc::AllocZero
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE void* CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::AllocZero( size numBytes, uint32 alignment /* = 0 */ )
{
	PROFILER_SCOPE_FUNC();
	void* pData = Alloc( numBytes, alignment, NULL );
	Mem_Memzero( pData, numBytes );
	return pData;
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
	alignment		 = S_Max<uint32>( numBytes >= defaultAlignment ? defaultAlignment : minAlignment, alignment );
	size alignedSize = S_Align<size>( numBytes, alignment );
	AssertMsg( alignedSize <= blockSize, "A frame allocator can't allocate memory more then block size" );

	// Try to allocate memory in the current block, moving forward through already recycled blocks
	// and growing the pool only when none of them is left
	memoryPool_t& pool = pools[currentPoolId];
	while ( true )
	{
		// Allocate a new block if we ran out of them
		if ( pool.currentBlockIt == pool.blockList.end() )
		{
			pool.currentBlockIt = AllocBlock( pool );
		}

		// Try to allocate memory in the block
		memoryBlock_t& block			  = *pool.currentBlockIt;
		uint64		   freeSize			  = blockSize - block.usedSize;
		byte*		   pFreeMemory		  = block.pData + block.usedSize;
		byte*		   pAlignedFreeMemory = S_Align<byte*>( pFreeMemory, alignment );
		uint64		   alignPadding		  = (uint64)( pAlignedFreeMemory - pFreeMemory );
		if ( alignedSize + alignPadding <= freeSize )
		{
			if ( pBlock )
			{
				*pBlock = &block;
			}

			block.usedSize += alignedSize + alignPadding;
			return pAlignedFreeMemory;
		}

		// Not enough room left in the block. If the block was empty the request
		// can never be satisfied and moving forward would grow the pool forever
		if ( block.usedSize == 0 )
		{
			Sys_Error( "A frame allocator can't allocate %llu bytes with alignment %i (block size: %i, pool: %i, name: '%s')", numBytes, alignment, blockSize, pool.id, pAllocName );
			return NULL;
		}

		// Not enough room left in the block
		++pool.currentBlockIt;
	}

	// If we here it is fatal error
	AssertNoEntry();
	return NULL;
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
FORCEINLINE void CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::MarkAsFreePool( uint32 poolId )
{
	// Reset used size and destruct objects in blocks
	PROFILER_SCOPE_FUNC();
	memoryPool_t& pool = pools[poolId];
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

	// Reset current block to the begin and mark the pool as free
	pool.currentBlockIt = pool.blockList.begin();
	pools[poolId].bIsFree.store( true, eastl::memory_order_release );
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
CFrameAlloc::GetTotalUsedSize
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE uint64 CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::GetTotalUsedSize( uint32 poolId ) const
{
	PROFILER_SCOPE_FUNC();
	uint64				totalUsedSize = 0;
	const memoryPool_t& pool		  = pools[poolId];
	for ( auto it = pool.blockList.begin(), itEnd = pool.blockList.end(); it != itEnd; ++it )
	{
		const memoryBlock_t& curBlock = *it;
		totalUsedSize += curBlock.usedSize;
	}
	return totalUsedSize;
}

/*
==================
CFrameAlloc::GetAllocName
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE const char* CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::GetAllocName() const
{
	return pAllocName;
}

/*
==================
CFrameAlloc::AllocBlock
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
FORCEINLINE typename CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::memoryBlockListIt_t CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::AllocBlock( memoryPool_t& pool )
{
	PROFILER_SCOPE_FUNC();
	memoryBlock_t& block = pool.blockList.emplace_back();
	block.usedSize		 = 0;
	block.pData			 = (byte*)Mem_MallocZero( blockSize );
	Msg( "FrameAlloc: Allocated block (%llu bytes, pool: %i, name: '%s')", blockSize, pool.id, pAllocName );
	PROFILER_MEM_ALLOC( block.pData, blockSize, pAllocName );
	return --pool.blockList.end();
}

/*
==================
CFrameAlloc::FreeBlock
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
typename CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::memoryBlockListIt_t CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::FreeBlock( memoryPool_t& pool, memoryBlockListIt_t blockIt )
{
	// Call destructors
	PROFILER_SCOPE_FUNC();
	Assert( blockIt != pool.blockList.end() );
	memoryBlock_t& block = *blockIt;
	for ( auto itDestructorEntry = block.destructorList.begin(), itDestructorEntryEnd = block.destructorList.end(); itDestructorEntry != itDestructorEntryEnd; ++itDestructorEntry )
	{
		destructorEntry_t& destructorEntry = *itDestructorEntry;
		destructorEntry.pDestroyObjectFn( destructorEntry.pObject );
	}

	// Free allocated memory for the block
	if ( block.pData )
	{
		Mem_Free( block.pData );
		PROFILER_MEM_FREE( block.pData, pAllocName );
	}

	// Remove the block from the list
	bool				bCurrentBlock = pool.currentBlockIt == blockIt;
	memoryBlockListIt_t nextBlockIt	  = pool.blockList.erase( blockIt );
	if ( bCurrentBlock )
	{
		pool.currentBlockIt = nextBlockIt;
	}

	// We are done
	Msg( "FrameAlloc: Freed block (%llu bytes, pool: %i, name: '%s')", blockSize, pool.id, pAllocName );
	return nextBlockIt;
}

/*
==================
CFrameAlloc::FreeAllBlocks
==================
*/
template<uint32 blockSize, uint32 numPools, uint32 defaultAlignment, uint32 minAlignment>
void CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::FreeAllBlocks( memoryPool_t& pool )
{
	// Do nothing if the list is empty
	PROFILER_SCOPE_FUNC();
	if ( pool.blockList.empty() )
	{
		return;
	}

	// Free each block
	for ( memoryBlockListIt_t it = pool.blockList.begin(); it != pool.blockList.end(); )
	{
		it = FreeBlock( pool, it );
	}
	pool.currentBlockIt = pool.blockList.end();
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

/*
==================
CStlFrameAlloc::CStlFrameAlloc
==================
*/
template<auto& frameAlloc>
FORCEINLINE CStlFrameAlloc<frameAlloc>::CStlFrameAlloc( const char* pName )
{
}

/*
==================
CStlFrameAlloc::CStlFrameAlloc
==================
*/
template<auto& frameAlloc>
FORCEINLINE CStlFrameAlloc<frameAlloc>::CStlFrameAlloc( const CStlFrameAlloc& other )
{
}

/*
==================
CStlFrameAlloc::CStlFrameAlloc
==================
*/
template<auto& frameAlloc>
FORCEINLINE CStlFrameAlloc<frameAlloc>::CStlFrameAlloc( const CStlFrameAlloc& other, const char* pName )
{
}

/*
==================
CStlFrameAlloc::allocate
==================
*/
template<auto& frameAlloc>
FORCEINLINE void* CStlFrameAlloc<frameAlloc>::allocate( size numBytes, int32 flags )
{
	return frameAlloc.Alloc( numBytes );
}

/*
==================
CStlFrameAlloc::allocate
==================
*/
template<auto& frameAlloc>
FORCEINLINE void* CStlFrameAlloc<frameAlloc>::allocate( size numBytes, size alignment, size offset, int32 flags )
{
	return frameAlloc.Alloc( numBytes, alignment );
}

/*
==================
CStlFrameAlloc::deallocate
==================
*/
template<auto& frameAlloc>
FORCEINLINE void CStlFrameAlloc<frameAlloc>::deallocate( void* pPtr, size numBytes )
{
}

/*
==================
CStlFrameAlloc::set_name
==================
*/
template<auto& frameAlloc>
FORCEINLINE void CStlFrameAlloc<frameAlloc>::set_name( const char* pName )
{
}

/*
==================
CStlFrameAlloc::get_name
==================
*/
template<auto& frameAlloc>
FORCEINLINE const char* CStlFrameAlloc<frameAlloc>::get_name() const
{
	return frameAlloc.GetAllocName();
}

/*
==================
CStlFrameAlloc::operator=
==================
*/
template<auto& frameAlloc>
FORCEINLINE CStlFrameAlloc<frameAlloc>& CStlFrameAlloc<frameAlloc>::operator=( const CStlFrameAlloc& other )
{
	return *this;
}
