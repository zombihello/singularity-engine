/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef STUDIO_RENDERPASS_PRESENT_H
#define STUDIO_RENDERPASS_PRESENT_H

#include "materialsystem/imaterial.h"

// Forward declarations
class IStudioAPIBoundShaderState;
class IStudioAPIRenderPipeline;
class IStudioAPISwapChain;
class CStudioViewport;

/**
 * @ingroup studiorender
 * @brief Studio present render pass
 */
class CStudioRenderPassPresent
{
public:
	/**
	 * @brief Initialize the pass
	 */
	void Init();

	/**
	 * @brief Shutdown the pass
	 */
	void Shutdown();

	/**
	 * @brief Submit a quad to drow (FOR TEST ONLY)
	 * @param pMaterial			Material
	 * @param pVertexBuffer		Vertex buffer
	 * @param pIndexBuffer		Index buffer
	 */
	FORCEINLINE void R_SubmitQuad( IMaterial* pMaterial, IStudioAPIBuffer* pVertexBuffer, IStudioAPIBuffer* pIndexBuffer )
	{
		pQuadVertexBuffer	= pVertexBuffer;
		pQuadIndexBuffer	= pIndexBuffer;
		pQuadMaterial		= pMaterial;
	}

	/**
	 * @brief Draw the pass
	 * @warning This is only called by the render thread
	 * @param pViewport		Viewport
	 */
	void R_DrawPass( CStudioViewport* pViewport );

	/**
	 * @brief Create a StudioAPI render pipeline
	 * @warning Must be called from the render thread
	 * @param pViewport						Viewport which for will be created a render pipeline
	 * @param pStudioAPIBoundShaderState	StudioAPI bound shader state
	 * @return Return created a new StudioAPI render pipeline
	 */
	static TRefPtr<IStudioAPIRenderPipeline> R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState );

private:
	// --- FOR TEST ONLY! ---
	TRefPtr<IStudioAPIBuffer>	pQuadVertexBuffer;	/**< StudioAPI quad vertex buffer */
	TRefPtr<IStudioAPIBuffer>	pQuadIndexBuffer;	/**< StudioAPI quad index buffer */
	TRefPtr<IMaterial>			pQuadMaterial;		/**< Quad material */
};

#endif // !STUDIO_RENDERPASS_PRESENT_H