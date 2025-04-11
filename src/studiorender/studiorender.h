/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef STUDIORENDER_H
#define STUDIORENDER_H

#include <stack>

#include "materialsystem/imaterial.h"
#include "studiorender/istudiorender.h"
#include "studiorender/studioapi/istudioapi_swapchain.h"
#include "studiorender/studio_renderpass_present.h"

// Forward declarations
class CStudioViewport;

/**
 * @ingroup studiorender
 * @brief Studio render
 */
class CStudioRender : public CBaseAppSystem<IStudioRender>
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioRender();

	/**
	 * @brief Connect application system
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE
	 */
	virtual bool Connect( createInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Query interface
	 *
	 * Here's where systems can access other interfaces implemented by this object
	 *
	 * @param pInterfaceName	Interface name
	 * @return Return pointer to interface, if doesn't implement the requested interface return NULL
	 */
	virtual void* QueryInterface( const achar* pInterfaceName ) override;

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() override;

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Set Studio API
	 * Sets which API we should be using. Has to be done before connect
	 *
	 * @param pStudioAPIDLL		Studio API module name
	 * @return Return TRUE if Studio API successfully set, otherwise returns FALSE
	 */
	virtual void SetStudioAPI( const achar* pStudioAPIDLL ) override;

	/**
	 * @brief Draw a quad (FOR TEST ONLY!)
	 * @param pMaterial		Material
	 * @param pVertexBuffer	Vertex buffer
	 * @param pIndexBuffer	Index buffer
	 */
	virtual void DrawQuad( IMaterial* pMaterial, IStudioAPIBuffer* pVertexBuffer, IStudioAPIBuffer* pIndexBuffer ) override;

	/**
	 * @brief Create a viewport
	 * @return Return created a viewport
	 */
	virtual IStudioViewport* CreateViewport() const override;

	/**
	 * @brief Create a render pipeline set
	 * @return Return created a render pipeline set
	 */
	virtual IStudioRenderPipelineSet* CreateRenderPipelineSet() const override;

	/**
	 * @brief Begin a frame
	 * @warning This is only called by the main thread
	 */
	void BeginFrame();

	/**
	 * @brief End the frame
	 * @warning This is only called by the main thread
	 */
	void EndFrame();

	/**
	 * @brief Draw a frame
	 * @warning This is only called by the render thread
	 * 
	 * @param pViewport		Viewport
	 */
	void R_DrawFrame( CStudioViewport* pViewport );

	/**
	 * @brief Get command buffer of the render thread
	 * @return Return pointer to command buffer of render thread. If return NULL it's mean what StudioRender don't use render thread
	 */
	virtual IStudioCmdBuffer* GetCommandBuffer() const override;

	/**
	 * @brief Is current thread is the render
	 * @return Return TRUE if it called from the render thread or render thread isn't use, otherwise returns FALSE
	 */
	virtual bool IsInRenderThread() const override;

private:
	/**
	 * @brief Creates the studio API implementation
	 * 
	 * @param pStudioAPIDLL		Studio API module name
	 * @return Return Studio API interface factory if module loaded, otherwise returns NULL
	 */
	createInterfaceFn_t CreateStudioAPI( const achar* pStudioAPIDLL );

	/**
	 * @brief Destroys the studio API implementation
	 */
	void DestroyStudioAPI();

	std::string					studioAPIDLLName;	/**< Studio API dll name */
	dllHandle_t					studioAPIHandle;	/**< Studio API handle */
	createInterfaceFn_t			pStudioAPIFactory;	/**< StudioAPI factory */
	createInterfaceFn_t			pAppSystemFactory;	/**< Application systems factory. It used for connect sutdiorender's submodules (e.g: studioapi) */
	CStudioRenderPassPresent	presentRenderPass;	/**< Present render pass */
};

/**
 * @ingroup studiorender
 * @brief Studio render
 */
extern CStudioRender		g_StudioRender;

#endif // !STUDIORENDER_H