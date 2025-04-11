/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef ISTUDIO_VIEWPORTCLIENT_H
#define ISTUDIO_VIEWPORTCLIENT_H

#include "studiorender/studioapi/istudioapi_swapchain.h"

// Forward declarations
class IStudioViewport;

/**
 * @ingroup studiorender
 * @brief Studio viewport client interface
 */
class IStudioViewportClient
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioViewportClient() {}

	/**
	 * @brief Update viewport client logic
	 * @param deltaSeconds		Delta time since the last frame (in seconds)
	 */
	virtual void Update( float deltaSeconds ) = 0;

	/**
	 * @brief Draw frame
	 * @param pStudioViewport	Viewport where draw a frame
	 */
	virtual void DrawFrame( IStudioViewport* pStudioViewport ) = 0;

	/**
	 * @brief Part of DrawFrame() function that executes at the render thread and before drawing of the scene
	 * @warning This is only called by the render thread
	 * 
	 * @param pStudioViewport	Viewport where draw a frame
	 */
	virtual void R_BeginDrawFrame( IStudioViewport* pStudioViewport ) = 0;

	/**
	 * @brief Part of DrawFrame() function that executes at the render thread and before present of the frame
	 * @warning This is only called by the render thread
	 *
	 * @param pStudioViewport	Viewport where draw a frame
	 */
	virtual void R_EndDrawFrame( IStudioViewport* pStudioViewport ) = 0;
};

/**
 * @ingroup studiorender
 * @brief Base implementation of a studio viewport client
 */
template<class TBaseClass>
class TBaseStudioViewportClient : public TBaseClass
{
public:
	/**
	 * @brief Update viewport client logic
	 * @param deltaSeconds		Delta time since the last frame (in seconds)
	 */
	virtual void Update( float deltaSeconds ) override {}

	/**
	 * @brie Draw frame
	 * @param pStudioViewport	Viewport where draw a frame
	 */
	virtual void DrawFrame( class IStudioViewport* pStudioViewport ) override {}

	/**
 * @brief Part of DrawFrame() function that executes at the render thread and before drawing of the scene
 * @warning This is only called by the render thread
 *
 * @param pStudioViewport	Viewport where draw a frame
 */
	virtual void R_BeginDrawFrame( IStudioViewport* pStudioViewport ) override {}

	/**
	 * @brief Part of DrawFrame() function that executes at the render thread and before present of the frame
	 * @warning This is only called by the render thread
	 *
	 * @param pStudioViewport	Viewport where draw a frame
	 */
	virtual void R_EndDrawFrame( IStudioViewport* pStudioViewport ) override {}
};

#endif // !ISTUDIO_VIEWPORTCLIENT_H