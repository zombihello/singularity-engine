/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef STUDIO_RENDERPIPELINESET_H
#define STUDIO_RENDERPIPELINESET_H

#include <vector>
#include "studiorender/istudio_renderpipelineset.h"
#include "studiorender/studio_viewport.h"

/**
 * @ingroup studiorender
 * @brief A studio render pipeline set
 */
class CStudioRenderPipelineSet : public TRefCounted<IStudioRenderPipelineSet>, public TStudioRenderResource<IStudioRenderResource>
{
public:
	/**
	 * @brief Initialize the render pipeline set
	 */
	virtual void Init() override;

	/**
	 * @brief Shutdown the render pipeline set
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Bake a render pipeline at index
	 * @warning Must be called from the render thread and in R_*DrawFrame functions
	 * @param bakeParams	Bake parameters
	 * @return Return a new baked render pipeline
	 */
	virtual IStudioAPIRenderPipeline* R_BakeRenderPipeline( const studioBakeRenderPipelineParams_t& bakeParams ) override;

	/**
	 * @brief Get StudioAPI render pipeline by render pass type and index
	 * @param renderPassType	Render pass type
	 * @param pipelineIdx		Pipeline index
	 * @return Return StudioAPI render pipeline at index for the render pass type. If the pipeline isn't baked return NULL
	 */
	virtual IStudioAPIRenderPipeline* GetStudioAPIRenderPipeline( studioRenderPassType_t renderPassType, uint64 pipelineIdx ) const override;

private:
	/**
	 * @brief Viewport render pipelines
	 */
	struct viewportRenderPipelines_t
	{
		/**
		 * @brief Constructor
		 */
		viewportRenderPipelines_t()
			: pViewport( NULL )
			, pReleaseViewportIndexDelegate( NULL )
			, pRenderPassUpdatedDelegate( NULL )
		{}

		CStudioViewport*											pViewport;						/**< Viewport */
		CStudioViewport::COnReleaseViewportIndex::funcDelegate_t*	pReleaseViewportIndexDelegate;	/**< Release viewport index delegate */
		CStudioViewport::COnRenderPassUpdated::funcDelegate_t*		pRenderPassUpdatedDelegate;		/**< Render pass updated delegate */
		std::vector<TRefPtr<IStudioAPIRenderPipeline>>				studioAPIRenderPipelines;		/**< Render pipelines */
	};

	/**
	 * @brief Data storage for draw passes
	 */
	struct dataStorageDrawRenderPasses_t
	{
		std::vector<TRefPtr<IStudioAPIRenderPipeline>>		studioAPIRenderPipelines;	/**< Render pipelines */
	};

	/**
	 * @brief Data storage for present pass
	 */
	struct dataStoragePresentPass_t
	{
		std::vector<viewportRenderPipelines_t>				viewports;	/**< Viewports */
	};

	/**
	 * @brief A render pipeline container
	 */
	class CRenderPipelineContainer
	{
	public:
		/**
		 * @brief Constructor
		 */
		CRenderPipelineContainer();

		/**
		 * @brief Destructor
		 */
		~CRenderPipelineContainer();

		/**
		 * @brief Initialize the container
		 * @param renderPassType	Render pass type
		 */
		void Init( studioRenderPassType_t renderPassType );

		/**
		 * @brief Destroy the container (the main thread side)
		 */
		void Destroy();

		/**
		 * @brief Bake a render pipeline at index
		 * @warning Must be called from the render thread
		 * @param bakeParams	Bake parameters
		 * @return Return a new baked render pipeline
		 */
		IStudioAPIRenderPipeline* R_BakeRenderPipeline( const studioBakeRenderPipelineParams_t& bakeParams );

		/**
		 * @brief Get StudioAPI render pipeline by index
		 * @param pipelineIdx		Pipeline index
		 * @return Return StudioAPI render pipeline at index. If the pipeline isn't baked return NULL
		 */
		IStudioAPIRenderPipeline* GetStudioAPIRenderPipeline( uint64 pipelineIdx ) const;

	private:
		/**
		 * @brief Handle event of release viewport index
		 * @param pUserData		Pointer to CRenderPipelineContainer
		 * @param pViewport		Viewport
		 */
		static void OnReleaseViewportIndex( void* pUserData, CStudioViewport* pViewport );

		/**
		 * @brief Handle event of a viewport render pass updated
		 * @param pUserData		Pointer to CRenderPipelineContainer
		 * @param pViewport		Viewport
		 */
		static void OnRenderPassUpdated( void* pUserData, CStudioViewport* pViewport );

		studioRenderPassType_t	renderPassType;		/**< Render pass type */
		void*					pDataStorage;		/**< Data storage */
	};

	CRenderPipelineContainer	renderPipelineContainers[STUDIO_RENDERPASS_NUM_TYPES]		/**< Render pipeline containers for each render pass type */;
};

#endif // !STUDIO_RENDERPIPELINESET_H