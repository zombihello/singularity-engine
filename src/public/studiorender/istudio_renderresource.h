#pragma once
#include <EASTL/unordered_set.h>

#include "utils/interfaces/interfaces.h"
#include "tier1/defines.h"
#include "tier1/types.h"
#include "tier0/threading.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/istudiorender.h"

//-----------------------------------------------------------------------------
// A render resource interface which is owned by the render thread
//-----------------------------------------------------------------------------
class IStudioRenderResource
{
public:
	virtual void InitResource()			  = 0;
	virtual void ReleaseResource()		  = 0;
	virtual void UpdateResource()		  = 0;
	virtual bool IsInitedResource() const = 0;

protected:
	virtual void InitStudioAPI()	= 0;
	virtual void ReleaseStudioAPI() = 0;
	virtual void UpdateStudioAPI()	= 0;
};

//-----------------------------------------------------------------------------
// Studio functions
//-----------------------------------------------------------------------------
void Studio_BeginInitResource( IStudioRenderResource* pResource );
void Studio_BeginUpdateResource( IStudioRenderResource* pResource );
void Studio_BeginReleaseResource( IStudioRenderResource* pResource );

template<class TStudioRenderResourceClass>
void Studio_BeginInitResourceSafe( TRefPtr<TStudioRenderResourceClass> pResource );

template<class TStudioRenderResourceClass>
void Studio_BeginUpdateResourceSafe( TRefPtr<TStudioRenderResourceClass> pResource );

template<class TStudioRenderResourceClass>
void Studio_BeginReleaseResourceSafe( TRefPtr<TStudioRenderResourceClass> pResource );

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
class TStudioRenderResource : public TBaseClass
{
public:
	// IStudioRenderResource interface
	virtual void InitResource() override;
	virtual void ReleaseResource() override;
	virtual void UpdateResource() override;
	virtual bool IsInitedResource() const override;

	TStudioRenderResource()
		: bInitedResource( false )
	{
		if ( bGlobal )
		{
			CStudioGlobalRenderResources::AddResource( this );
		}
	}
	virtual ~TStudioRenderResource()
	{
		if ( bGlobal )
		{
			CStudioGlobalRenderResources::RemoveResource( this );
		}

		if ( !bInitedResource )
		{
			return;
		}

		// Deleting an initialized IStudioRenderResource will result in a crash later since it is still linked
		AssertMsg( false, "An IStudioRenderResource was deleted without being released first!" );
	}

protected:
	virtual void InitStudioAPI() override;
	virtual void ReleaseStudioAPI() override;
	virtual void UpdateStudioAPI() override;

private:
	volatile bool bInitedResource;
};

#include "studiorender/istudio_renderresource.inl"
