#ifndef VK_STUDIOAPI_INL
#define VK_STUDIOAPI_INL

/*
==================
CStudioAPIVk::GetDevice
==================
*/
FORCEINLINE const CStudioAPIDeviceVk& CStudioAPIVk::GetDevice() const
{
	return device;
}

/*
==================
CStudioAPIVk::GetMemoryMgr
==================
*/
FORCEINLINE const CStudioAPIMemoryMgrVk& CStudioAPIVk::GetMemoryMgr() const
{
	return memoryMgr;
}

/*
==================
CStudioAPIVk::OnStudioAPIVkShutdown
==================
*/
FORCEINLINE COnStudioAPIVkShutdown& CStudioAPIVk::OnStudioAPIVkShutdown() const
{
	return onStudioAPIVkShutdown;
}

/*
==================
CStudioAPIVk::GetBoundShaderStateCache
==================
*/
FORCEINLINE CStudioAPIBoundShaderStateCacheVk& CStudioAPIVk::GetBoundShaderStateCache()
{
	return boundShaderStateCache;
}

/*
==================
CStudioAPIVk::GetDescriptorPoolsMgr
==================
*/
FORCEINLINE CStudioAPIDescriptorPoolsMgrVk& CStudioAPIVk::GetDescriptorPoolsMgr()
{
	return descriptorPoolsMgr;
}

/*
==================
CStudioAPIVk::GetDataUploader
==================
*/
FORCEINLINE CStudioAPIDataUploaderVk& CStudioAPIVk::GetDataUploader()
{
	return dataUploader;
}

/*
==================
CStudioAPIVk::GetSyncMgr
==================
*/
FORCEINLINE CStudioAPISyncMgrVk& CStudioAPIVk::GetSyncMgr()
{
	return syncMgr;
}

/*
==================
CStudioAPIVk::GetCurrentFrameInFlight
==================
*/
FORCEINLINE uint32 CStudioAPIVk::GetCurrentFrameInFlight() const
{
	return currentFrameInFlight;
}

#endif // !VK_STUDIOAPI_INL