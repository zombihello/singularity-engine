/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef STUDIO_VERTEXDECLARATIONS_H
#define STUDIO_VERTEXDECLARATIONS_H

#include "studiorender/studioapi/istudioapi_shader.h"
#include "studiorender/istudio_renderresource.h"
#include "studiorender/studio_vertextypes.h"

/**
 * @ingroup studiorender
 * @brief Studio vertex declaration for each vertex type 
 */
class CStudioVertexDeclarations : public TStudioRenderResource<IStudioRenderResource, true>
{
public:
	/**
	 * @brief Initialize vertex declarations
	 */
	FORCEINLINE void Init()
	{
		Assert( !IsInitedResource() );
		Studio_BeginInitResource( this );
	}

	/**
	 * @brief Destroy the vertex declarations
	 */
	FORCEINLINE void Destroy()
	{
		Assert( IsInitedResource() );
		Studio_BeginReleaseResource( this );
	}

	/**
	 * @brief Get StudioAPI vertex declaration for specific vertex type
	 * @param vertexType	Studio vertex type
	 * @return Return StudioAPI vertex declaration for the vertex type. If the vertex declarations isn't initialized return NULL
	 */
	FORCEINLINE IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration( studioVertexType_t vertexType ) const
	{
		Assert( vertexType < STUDIO_VERTEX_NUM_TYPES );
		return pStudioAPIVertexDeclarations[vertexType];
	}

private:
	/**
	 * @brief Initializes the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 *
	 * Called when the resource is initialized
	 */
	virtual void InitStudioAPI() override;

	/**
	 * @brief Releases the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 *
	 * Called when the resource is released
	 */
	virtual void ReleaseStudioAPI() override;

	TRefPtr<IStudioAPIVertexDeclaration>		pStudioAPIVertexDeclarations[STUDIO_VERTEX_NUM_TYPES];	/**< StudioAPI vertex declaration for each vertex type */
};

/**
 * @ingroup studiorender
 * @brief Studio vertex declarations
 */
extern CStudioVertexDeclarations		g_StudioVertexDeclarations;

#endif // !STUDIO_VERTEXDECLARATIONS_H