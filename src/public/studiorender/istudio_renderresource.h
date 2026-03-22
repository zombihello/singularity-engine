#pragma once
#include <EASTL/atomic.h>
#include <EASTL/unordered_set.h>

#include "utils/interfaces/interfaces.h"
#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/threading.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/istudiorender.h"

//-----------------------------------------------------------------------------
// A render resource interface which is owned by the render thread
//-----------------------------------------------------------------------------
class IStudioRenderResource
{
public:
	virtual ~IStudioRenderResource() {}

	// Render thread interface
	virtual void InitResource()	   = 0;
	virtual void ReleaseResource() = 0;
	virtual void UpdateResource()  = 0;

	// Queue/lifetime control
	virtual void BeginEnqueueRenderOp()		   = 0;
	virtual void EndEnqueueRenderOp()		   = 0;
	virtual bool IsNeedDeferredDestroy() const = 0;
	virtual bool IsInitedResource() const	   = 0;

protected:
	virtual void InitStudioAPI()	= 0;
	virtual void ReleaseStudioAPI() = 0;
	virtual void UpdateStudioAPI()	= 0;
};

//-----------------------------------------------------------------------------
// Container for all global render resources in the module
//-----------------------------------------------------------------------------
class CStudioGlobalRenderResources
{
public:
	static void InitResources();
	static void ReleaseResources();
	static void AddResource( IStudioRenderResource* pResource );
	static void RemoveResource( IStudioRenderResource* pResource );

	static eastl::unordered_set<IStudioRenderResource*>& GetResourceList();

private:
	static CThreadMutex& GetThreadMutex();
};

//-----------------------------------------------------------------------------
// Base class to implement a render resource
// NOTE: For use g_pStudioAPI must be valid
//-----------------------------------------------------------------------------
template<class TBaseClass, bool bGlobal = false>
class CStudioRenderResource : public TBaseClass
{
public:
	// IStudioRenderResource interface
	virtual void InitResource() override;
	virtual void ReleaseResource() override;
	virtual void UpdateResource() override;

	virtual void BeginEnqueueRenderOp() override;
	virtual void EndEnqueueRenderOp() override;
	virtual bool IsNeedDeferredDestroy() const override;
	virtual bool IsInitedResource() const override;

	CStudioRenderResource();
	virtual ~CStudioRenderResource();

protected:
	virtual void InitStudioAPI() override;
	virtual void ReleaseStudioAPI() override;
	virtual void UpdateStudioAPI() override;

private:
	eastl::atomic<uint32> numPendingRenderOps;
	eastl::atomic<bool>	  bInitedResource;
};

//-----------------------------------------------------------------------------
// Functions to send a message to the rendering thread
//-----------------------------------------------------------------------------
void Studio_BeginInitResource( IStudioRenderResource* pResource );
void Studio_BeginUpdateResource( IStudioRenderResource* pResource );
void Studio_BeginReleaseResource( IStudioRenderResource* pResource );
void Studio_BeginDeleteResource( IStudioRenderResource* pResource );

#include "studiorender/istudio_renderresource.inl"
