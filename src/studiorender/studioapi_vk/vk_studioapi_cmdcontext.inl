#pragma once

/*
==================
CStudioAPICmdContextVk::GetQueue
==================
*/
FORCEINLINE CStudioAPIQueueVk& CStudioAPICmdContextVk::GetQueue()
{
	return queue;
}

/*
==================
CStudioAPICmdContextVk::GetCmdBufferPool
==================
*/
FORCEINLINE CStudioAPICmdBufferPoolVk& CStudioAPICmdContextVk::GetCmdBufferPool()
{
	return cmdBufferPools[g_StudioAPIVk.GetCurrentFrameInFlight()];
}

/*
==================
CStudioAPICmdContextVk::AddSupportQueueTypes
==================
*/
FORCEINLINE void CStudioAPICmdContextVk::AddSupportQueueTypes( uint8 type )
{
	supportQueueTypes |= type;
}

/*
==================
CStudioAPICmdContextVk::HasSupportQueueTypes
==================
*/
FORCEINLINE bool CStudioAPICmdContextVk::HasSupportQueueTypes( uint8 types ) const
{
	return ( supportQueueTypes & types ) == types;
}

/*
==================
CStudioAPICmdContextVk::GetSupportQueueTypes
==================
*/
FORCEINLINE uint8 CStudioAPICmdContextVk::GetSupportQueueTypes() const
{
	return supportQueueTypes;
}

/*
==================
CStudioAPICmdContextVk::GetCmdListBatchMgr
==================
*/
FORCEINLINE CStudioAPICmdListBatchMgrVk& CStudioAPICmdContextVk::GetCmdListBatchMgr()
{
	return cmdListBatchMgr;
}
