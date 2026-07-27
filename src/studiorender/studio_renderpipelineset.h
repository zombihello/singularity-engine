#pragma once
#include <EASTL/vector.h>

#include "studiorender/istudio_renderpipelineset.h"
#include "studiorender/studio_viewport.h"

//-----------------------------------------------------------------------------
// A studio render pipeline set
//-----------------------------------------------------------------------------
class CStudioRenderPipelineSet : public CRefCounted<IStudioRenderPipelineSet>
{
public:
	// IStudioRenderPipelineSet interface
	virtual IStudioAPIRenderPipeline* R_BakeRenderPipeline( const studioBakeRenderPipelineParams_t& bakeParams ) override;
	virtual IStudioAPIRenderPipeline* R_GetStudioAPIRenderPipeline( studioRenderPassType_t renderPassType, uint64 renderStateIdx, uint64 shaderComboIdx ) const override;

	CStudioRenderPipelineSet();
	~CStudioRenderPipelineSet();

private:
	using renderPipelines_t = eastl::vector<CRefPtr<IStudioAPIRenderPipeline>>;	 // Indexed by shaderComboIdx
	using renderStates_t	= eastl::vector<renderPipelines_t>;					 // Indexed by renderStateIdx

	struct viewportRenderPipelines_t
	{
		viewportRenderPipelines_t()
			: pViewport( NULL )
			, onReleaseViewportIndexHandle( INVALID_HANDLE )
			, onRenderPassUpdatedHandle( INVALID_HANDLE )
		{
		}

		CStudioViewport*								   pViewport;
		CStudioViewport::COnReleaseViewportIndex::handle_t onReleaseViewportIndexHandle;
		CStudioViewport::COnRenderPassUpdated::handle_t	   onRenderPassUpdatedHandle;
		renderStates_t									   renderStates;
	};

	struct dataStorageDrawRenderPasses_t
	{
		renderStates_t renderStates;
	};

	struct dataStoragePresentPass_t
	{
		eastl::vector<viewportRenderPipelines_t> viewports;
	};

	class CRenderPipelineContainer
	{
	public:
		CRenderPipelineContainer();
		~CRenderPipelineContainer();

		void Init( studioRenderPassType_t renderPassType );
		void Destroy();

		IStudioAPIRenderPipeline* R_BakeRenderPipeline( const studioBakeRenderPipelineParams_t& bakeParams );
		IStudioAPIRenderPipeline* R_GetStudioAPIRenderPipeline( uint64 renderStateIdx, uint64 shaderComboIdx ) const;

	private:
		static void OnReleaseViewportIndex( void* pUserData, CStudioViewport* pViewport );
		static void OnRenderPassUpdated( void* pUserData, CStudioViewport* pViewport );

		studioRenderPassType_t renderPassType;
		void*				   pDataStorage;
	};

	CRenderPipelineContainer renderPipelineContainers[STUDIO_RENDERPASS_NUM_TYPES];
};
