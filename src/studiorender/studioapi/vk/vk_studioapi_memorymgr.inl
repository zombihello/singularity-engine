#pragma once

/*
==================
CStudioAPIMemoryMgrVk::FreeResource
==================
*/
template<typename TFreeLambda>
FORCEINLINE void CStudioAPIMemoryMgrVk::FreeResource( TFreeLambda&& freeLambda ) const
{
	cmdBuffer_t& currentCmdBuffer = GetCurrentPendingFreeCmdBuffer();
	auto				pendingFreeCmdFunc = []( void* pUserData )
		{
			auto	pFunc = ( TFreeLambda* )pUserData;
			( *pFunc )( );
			pFunc->~TFreeLambda();
		};

	AssertMsg( currentCmdBuffer.pWritePtr - currentCmdBuffer.pBuffer +
			   sizeof( pendingFreeResourceFn_t ) + sizeof( uint32 ) +
			   sizeof( TFreeLambda ) < currentCmdBuffer.size, "Not enough memory in the command buffer to write the command" );
	*( pendingFreeResourceFn_t* )currentCmdBuffer.pWritePtr = pendingFreeCmdFunc;
	currentCmdBuffer.pWritePtr += sizeof( pendingFreeResourceFn_t );

	*( uint32* )currentCmdBuffer.pWritePtr = sizeof( TFreeLambda );
	currentCmdBuffer.pWritePtr += sizeof( uint32 );

	new( currentCmdBuffer.pWritePtr ) TFreeLambda( std::forward<TFreeLambda>( ( TFreeLambda&& )freeLambda ) );
	currentCmdBuffer.pWritePtr += sizeof( TFreeLambda );
	++currentCmdBuffer.numCmds;
}

/*
==================
CStudioAPIMemoryMgrVk::FreePendingResources
==================
*/
FORCEINLINE void CStudioAPIMemoryMgrVk::FreePendingResources( uint32 indexFrameInFlight ) const
{
	ExecPendingFreeCmdBuffer( pendingFreeCmdBuffers[indexFrameInFlight] );
}

/*
==================
CStudioAPIMemoryMgrVk::MapMemory
==================
*/
template<typename TType>
FORCEINLINE TType* CStudioAPIMemoryMgrVk::MapMemory( VmaAllocation vmaAllocation ) const
{
	TType* pMappedMemory = NULL;
	STUDIOAPI_VK_VERIFY_RESULT( vmaMapMemory( vmaAllocator, vmaAllocation, ( void** )&pMappedMemory ) );
	return pMappedMemory;
}

/*
==================
CStudioAPIMemoryMgrVk::UnmapMemory
==================
*/
FORCEINLINE void CStudioAPIMemoryMgrVk::UnmapMemory( VmaAllocation vmaAllocation ) const
{
	vmaUnmapMemory( vmaAllocator, vmaAllocation );
}
