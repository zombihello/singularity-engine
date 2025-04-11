/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef ISTUDIORENDER_H
#define ISTUDIORENDER_H

#include "stdlib/refcount.h"
#include "appframework/iappsystem.h"
#include "studiorender/istudio_cmdbuffer.h"

// Forward declarations
class IStudioViewport;
class IStudioRenderPipelineSet;
class IMaterial;
class IStudioAPIBuffer;

/**
 * @ingroup studiorender
 * @brief Studio render interface version
 */
#define STUDIORENDER_INTERFACE_VERSION "SStudioRender001"

/**
 * @ingroup studiorender
 * @brief Studio render interface
 */
class IStudioRender : public IAppSystem
{
public:
	/**
	 * @brief Set Studio API
	 * Sets which API we should be using. Has to be done before connect
	 * 
	 * @param pStudioAPIDLL		Studio API module name (e.g studioapi_dx11.dll)
	 * @return Return TRUE if Studio API successfully set, otherwise returns FALSE
	 */
	virtual void SetStudioAPI( const achar* pStudioAPIDLL ) = 0;

	/**
	 * @brief Draw a quad (FOR TEST ONLY!)
	 * @param pMaterial		Material
	 * @param pVertexBuffer	Vertex buffer
	 * @param pIndexBuffer	Index buffer
	 */
	virtual void DrawQuad( IMaterial* pMaterial, IStudioAPIBuffer* pVertexBuffer, IStudioAPIBuffer* pIndexBuffer ) = 0;

	/**
	 * @brief Create a viewport
	 * @return Return created a viewport
	 */
	virtual IStudioViewport* CreateViewport() const = 0;

	/**
	 * @brief Create a render pipeline set
	 * @return Return created a render pipeline set
	 */
	virtual IStudioRenderPipelineSet* CreateRenderPipelineSet() const = 0;

	/**
	 * @brief Get command buffer of the render thread
	 * @return Return pointer to command buffer of render thread. If return NULL it's mean what StudioRender don't use render thread
	 */
	virtual IStudioCmdBuffer* GetCommandBuffer() const = 0;

	/**
	 * @brief Is current thread is the render
	 * @return Return TRUE if it called from the render thread or render thread isn't use, otherwise returns FALSE
	 */
	virtual bool IsInRenderThread() const = 0;
};

#endif // !ISTUDIORENDER_H