#pragma once

/*
==================
IStudioRenderCmd::operator new
==================
*/
FORCEINLINE void* IStudioRenderCmd::operator new( size size, const studioCmdAlloc_t& studioCmdAlloc )
{
	return studioCmdAlloc.pAllocation;
}

/*
==================
IStudioRenderCmd::operator delete
==================
*/
FORCEINLINE void IStudioRenderCmd::operator delete( void* pPtr, const studioCmdAlloc_t& studioCmdAlloc )
{
}

/*
==================
CStudioRenderCmd_Skip::Execute
==================
*/
FORCEINLINE uint32 CStudioRenderCmd_Skip::Execute()
{
	return numSkipBytes;
}

/*
==================
CStudioRenderCmd_Skip::GetSize
==================
*/
FORCEINLINE uint32 CStudioRenderCmd_Skip::GetSize() const
{
	return numSkipBytes;
}

/*
==================
CStudioRenderCmdFence::CStudioRenderCmdFence
==================
*/
FORCEINLINE CStudioRenderCmdFence::CStudioRenderCmdFence()
{
	numPendingFences.store( 0, eastl::memory_order_release );
}

/*
==================
CStudioRenderCmdFence::InsertFence
==================
*/
FORCEINLINE void CStudioRenderCmdFence::InsertFence()
{
	PROFILER_SCOPE_FUNC();
	numPendingFences.fetch_add( 1, eastl::memory_order_acq_rel );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioRenderCmd_Fence,
										CStudioRenderCmdFence*, pFence, this,
										{
											pFence->numPendingFences.fetch_sub( 1, eastl::memory_order_acq_rel );
										} );
}

/*
==================
CStudioRenderCmdFence::Wait
==================
*/
FORCEINLINE void CStudioRenderCmdFence::Wait( uint32 numFencesLeft ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_WAIT );
	Assert( Sys_IsInMainThread() );
	while ( GetNumPendingFences() > numFencesLeft )
	{
		Sys_Yield();
	}
}

/*
==================
CStudioRenderCmdFence::GetNumPendingFences
==================
*/
FORCEINLINE uint32 CStudioRenderCmdFence::GetNumPendingFences() const
{
	return numPendingFences.load( eastl::memory_order_acquire );
}

/*
==================
Studio_FlushRenderCommands
==================
*/
FORCEINLINE void Studio_FlushRenderCommands()
{
	if ( !g_pStudioRender->IsInRenderThread() )
	{
		g_pStudioRender->GetCommandBuffer()->Flush();
	}
}
