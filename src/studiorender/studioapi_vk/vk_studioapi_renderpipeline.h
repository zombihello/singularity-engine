#pragma once
#include "studiorender/studioapi/istudioapi_renderpipeline.h"
#include "studiorender/studioapi_vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi_vk/vk_studioapi_device.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpass.h"

//-----------------------------------------------------------------------------
// Vulkan Studio API render pipeline
//-----------------------------------------------------------------------------
class CStudioAPIRenderPipelineVk : public CRefCounted<IStudioAPIRenderPipeline>
{
public:
	CStudioAPIRenderPipelineVk( const studioAPIRenderPipelineCreateInfo_t& createInfo, const char* pDebugName = "" );
	~CStudioAPIRenderPipelineVk();

	FORCEINLINE VkPipeline							 GetVkPipeline() const { return vkPipeline; }
	FORCEINLINE CStudioAPIBoundShaderStateVk*		 GetBoundShaderState() const { return pBoundShaderState; }
	FORCEINLINE COnStudioAPIRenderPipelineDeletedVk& OnRenderPipelineDeleted() const { return onRenderPipelineDeleted; }

private:
	static void OnStudioAPIVkShutdown( void* pUserData );

	VkPipeline									vkPipeline;
	COnStudioAPIVkShutdown::handle_t			onStudioAPIVkShutdownHandle;
	CRefPtr<CStudioAPIBoundShaderStateVk>		pBoundShaderState;
	mutable COnStudioAPIRenderPipelineDeletedVk onRenderPipelineDeleted;
};
