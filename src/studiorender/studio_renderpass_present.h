#ifndef STUDIO_RENDERPASS_PRESENT_H
#define STUDIO_RENDERPASS_PRESENT_H

#include "materialsystem/imaterial.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIBoundShaderState;
class IStudioAPIRenderPipeline;
class IStudioAPISwapChain;
class CStudioViewport;


//-----------------------------------------------------------------------------
// Studio present render pass
//-----------------------------------------------------------------------------
class CStudioRenderPassPresent
{
public:
	void Init();
	void Shutdown();

	// NOTE: FOR TEST ONLY
	FORCEINLINE void R_SubmitQuad( IMaterial* pMaterial, IStudioAPIBuffer* pVertexBuffer, IStudioAPIBuffer* pIndexBuffer )
	{
		pQuadVertexBuffer	= pVertexBuffer;
		pQuadIndexBuffer	= pIndexBuffer;
		pQuadMaterial		= pMaterial;
	}

	void R_DrawPass( CStudioViewport* pViewport );
	static TRefPtr<IStudioAPIRenderPipeline> R_CreateStudioAPIRenderPipeline( CStudioViewport* pViewport, IStudioAPIBoundShaderState* pStudioAPIBoundShaderState );

private:
	// --- FOR TEST ONLY! ---
	TRefPtr<IStudioAPIBuffer>	pQuadVertexBuffer;
	TRefPtr<IStudioAPIBuffer>	pQuadIndexBuffer;
	TRefPtr<IMaterial>			pQuadMaterial;
};

#endif // !STUDIO_RENDERPASS_PRESENT_H