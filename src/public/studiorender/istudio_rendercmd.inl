#ifndef ISTUDIO_RENDERCMD_INL
#define ISTUDIO_RENDERCMD_INL

/*
==================
IStudioRenderCmd::operator new
==================
*/
FORCEINLINE void* IStudioRenderCmd::operator new( size_t size, const studioCmdAlloc_t& studioCmdAlloc )
{
	return studioCmdAlloc.pAllocation;
}

/*
==================
IStudioRenderCmd::operator delete
==================
*/
FORCEINLINE void IStudioRenderCmd::operator delete( void* pPtr, const studioCmdAlloc_t& studioCmdAlloc )
{}


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

#endif // !ISTUDIO_RENDERCMD_INL
