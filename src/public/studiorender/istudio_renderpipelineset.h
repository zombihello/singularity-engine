/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef ISTUDIO_RENDERPIPELINESET_H
#define ISTUDIO_RENDERPIPELINESET_H

#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/istudio_renderresource.h"
#include "studiorender/studio_renderpasstypes.h"
#include "studiorender/studio_vertextypes.h"

// Forward declarations
class IStudioAPIRenderPipeline;

/**
 * @ingroup studiorender
 * @brief Parameters to bake a render pipeline in IStudioRenderPipelineSet
 */
struct studioBakeRenderPipelineParams_t
{
	uint64						pipelineIdx;											/**< Pipeline index */
	studioRenderPassType_t		renderPassType;											/**< Render pass type */
	studioVertexType_t			vertexType;												/**< Vertex type */
	IStudioAPIShader*			pStudioAPIShaders[STUDIOAPI_SHADER_NUM_DRAW_TYPES];		/**< StudioAPI shaders */
};

/**
 * @ingroup studiorender
 * @brief A studio render pipeline set interface
 */
class IStudioRenderPipelineSet : public IRefCounted
{
public:
	/**
	 * @brief Initialize the render pipeline set
	 */
	virtual void Init() = 0;

	/**
	 * @brief Shutdown the render pipeline set
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Bake a render pipeline at index
	 * @warning Must be called from the render thread and in R_*DrawFrame functions
	 * @param bakeParams	Bake parameters
	 * @return Return a new baked render pipeline
	 */
	virtual IStudioAPIRenderPipeline* R_BakeRenderPipeline( const studioBakeRenderPipelineParams_t& bakeParams ) = 0;

	/**
	 * @brief Get StudioAPI render pipeline by render pass type and index
	 * @param renderPassType	Render pass type
	 * @param pipelineIdx		Pipeline index
	 * @return Return StudioAPI render pipeline at index for the render pass type. If the pipeline isn't baked return NULL
	 */
	virtual IStudioAPIRenderPipeline* GetStudioAPIRenderPipeline( studioRenderPassType_t renderPassType, uint64 pipelineIdx ) const = 0;
};

#endif // !ISTUDIO_RENDERPIPELINESET_H