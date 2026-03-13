#pragma once
#include <EASTL/vector.h>

#include "studiorender/istudio_renderpipelineset.h"
#include "studiorender/studio_viewport.h"

//-----------------------------------------------------------------------------
// A studio render pipeline set
//-----------------------------------------------------------------------------
class CStudioRenderPipelineSet : public CRefCounted<IStudioRenderPipelineSet>, public CStudioRenderResource<IStudioRenderResource>
{
public:
	// IStudioRenderPipelineSet interface
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual IStudioAPIRenderPipeline* R_BakeRenderPipeline( const studioBakeRenderPipelineParams_t& bakeParams ) override;
	virtual IStudioAPIRenderPipeline* GetStudioAPIRenderPipeline( studioRenderPassType_t renderPassType, uint64 pipelineIdx ) const override;

private:
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
		eastl::vector<CRefPtr<IStudioAPIRenderPipeline>>   studioAPIRenderPipelines;
	};

	struct dataStorageDrawRenderPasses_t
	{
		eastl::vector<CRefPtr<IStudioAPIRenderPipeline>> studioAPIRenderPipelines;
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
		IStudioAPIRenderPipeline* GetStudioAPIRenderPipeline( uint64 pipelineIdx ) const;

	private:
		static void OnReleaseViewportIndex( void* pUserData, CStudioViewport* pViewport );
		static void OnRenderPassUpdated( void* pUserData, CStudioViewport* pViewport );

		studioRenderPassType_t renderPassType;
		void*				   pDataStorage;
	};

	CRenderPipelineContainer renderPipelineContainers[STUDIO_RENDERPASS_NUM_TYPES];
};
