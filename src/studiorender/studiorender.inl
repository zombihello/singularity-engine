#pragma once

/*
==================
CStudioRender::AddResourceToSceneView
==================
*/
template<class TResourceClass>
FORCEINLINE uint32 CStudioRender::AddResourceToSceneView( studioSceneView_t* pSceneView, TResourceClass* pResource )
{
	static_assert( false, "Invalid TResourceClass, the class isn't corresponds to any studio resource type" );
	return INVALID_INDEX;
}

/*
==================
CStudioRender::AddResourceToSceneView
==================
*/
template<>
FORCEINLINE uint32 CStudioRender::AddResourceToSceneView<IModelResource>( studioSceneView_t* pSceneView, IModelResource* pModelResource )
{
	return AddResourceToSceneView( pSceneView, pModelResource, STUDIO_RESOURCE_TYPE_MODEL );
}

/*
==================
CStudioRender::AddResourceToSceneView
==================
*/
template<>
FORCEINLINE uint32 CStudioRender::AddResourceToSceneView<IMaterialResource>( studioSceneView_t* pSceneView, IMaterialResource* pMaterialResource )
{
	return AddResourceToSceneView( pSceneView, pMaterialResource, STUDIO_RESOURCE_TYPE_MATERIAL );
}

/*
==================
CStudioRender::GetStudioAPIGlobalConstantBuffer
==================
*/
FORCEINLINE IStudioAPIBuffer* CStudioRender::GetStudioAPIGlobalConstantBuffer() const
{
	return pStudioAPIGlobalConstantBuffer;
}

/*
==================
CStudioRender::GetSceneRenderTargets
==================
*/
FORCEINLINE const CStudioSceneRenderTargets& CStudioRender::GetSceneRenderTargets() const
{
	return sceneRenderTargets;
}

/*
==================
CStudioRender::GetSceneRenderTargets
==================
*/
FORCEINLINE CStudioRenderPassBase* CStudioRender::GetRenderPass( studioRenderPassType_t type ) const
{
	Assert( type < STUDIO_RENDERPASS_NUM_TYPES );
	return pRenderPasses[(uint32)type];
}

/*
==================
CStudioRender::GetBatchedSimpleElements
==================
*/
FORCEINLINE CStudioBatchedSimpleElements& CStudioRender::GetBatchedSimpleElements()
{
	return batchedSimpleElements;
}
