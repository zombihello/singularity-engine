#ifndef ISTUDIORENDER_H
#define ISTUDIORENDER_H

#include "stdlib/refcount.h"
#include "appframework/iappsystem.h"
#include "studiorender/istudio_cmdbuffer.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioViewport;
class IStudioRenderPipelineSet;
class IMaterial;
class IStudioAPIBuffer;
class IStudioScene;


//-----------------------------------------------------------------------------
// Studio render interface
//-----------------------------------------------------------------------------
#define STUDIORENDER_INTERFACE_VERSION		"SStudioRender001"
class IStudioRender : public IAppSystem
{
public:
	// Sets which API we should be using. Has to be done before connect
	virtual void SetStudioAPI( const achar* pStudioAPIDLL ) = 0;

	// NOTE: FOR TEST ONLY!
	virtual void DrawQuad( IMaterial* pMaterial, IStudioAPIBuffer* pVertexBuffer, IStudioAPIBuffer* pIndexBuffer ) = 0;

	virtual IStudioViewport* CreateViewport() const = 0;
	virtual IStudioRenderPipelineSet* CreateRenderPipelineSet() const = 0;
	virtual IStudioScene* CreateScene() const = 0;

	// Returns a command buffer of the render thread. If return NULL it's mean what StudioRender don't use render thread
	virtual IStudioCmdBuffer* GetCommandBuffer() const = 0;
	virtual bool IsInRenderThread() const = 0;
};

#endif // !ISTUDIORENDER_H