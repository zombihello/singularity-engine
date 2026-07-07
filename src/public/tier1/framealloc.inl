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
		uint64		   freeSize = blockSize - block.usedSize;
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
		Assert( blockSize >= alignedSize + alignPadding );

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

	// Mark the pool as free
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
FORCEINLINE void CFrameAlloc<blockSize, numPools, defaultAlignment, minAlignment>::AllocBlock( memoryPool_t& pool )
{
	PROFILER_SCOPE_FUNC();
	memoryBlock_t& block = pool.blockList.emplace_back();
	block.usedSize		 = 0;
	block.pData			 = (byte*)Mem_MallocZero( blockSize );
	Msg( "FrameAlloc: Allocated block (%llu bytes, pool: %i, name: '%s')", blockSize, pool.id, pAllocName );
	PROFILER_MEM_ALLOC( block.pData, blockSize, pAllocName );
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

			Msg( "FrameAlloc: Freed block (%llu bytes, pool: %i, name: '%s')", blockSize, pool.id, pAllocName );
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
	uint64 numBlocks = pool.blockList.size();
	for ( auto it = pool.blockList.begin(), itEnd = pool.blockList.end(); it != itEnd; ++it )
	{
		memoryBlock_t& curBlock = *it;
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

	Msg( "FrameAlloc: Freed %i blocks (%llu bytes, pool: %i, name: '%s')", numBlocks, (uint32)pool.blockList.size() * blockSize, pool.id, pAllocName );
	pool.blockList.clear();
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
