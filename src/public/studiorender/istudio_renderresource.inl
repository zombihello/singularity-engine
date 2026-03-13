#pragma once

/*
==================
Studio_BeginInitResource
==================
*/
FORCEINLINE void Studio_BeginInitResource( IStudioRenderResource* pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioInitResourceCmd, IStudioRenderResource*, pResource, pResource,
										{
											pResource->InitResource();
										} );
}

/*
==================
Studio_BeginUpdateResource
==================
*/
FORCEINLINE void Studio_BeginUpdateResource( IStudioRenderResource* pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioReleaseResourceCmd, IStudioRenderResource*, pResource, pResource,
										{
											pResource->UpdateResource();
										} );
}

/*
==================
Studio_BeginReleaseResource
==================
*/
FORCEINLINE void Studio_BeginReleaseResource( IStudioRenderResource* pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioUpdateResourceCmd, IStudioRenderResource*, pResource, pResource,
										{
											pResource->ReleaseResource();
										} );
}

/*
==================
Studio_BeginInitResourceSafe
==================
*/
template<class TStudioRenderResourceClass>
FORCEINLINE void Studio_BeginInitResourceSafe( CRefPtr<TStudioRenderResourceClass> pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioInitResourceCmd, CRefPtr<TStudioRenderResourceClass>, pResource, pResource,
										{
											pResource->InitResource();
										} );
}

/*
==================
Studio_BeginUpdateResourceSafe
==================
*/
template<class TStudioRenderResourceClass>
FORCEINLINE void Studio_BeginUpdateResourceSafe( CRefPtr<TStudioRenderResourceClass> pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioReleaseResourceCmd, CRefPtr<TStudioRenderResourceClass>, pResource, pResource,
										{
											pResource->UpdateResource();
										} );
}

/*
==================
Studio_BeginReleaseResourceSafe
==================
*/
template<class TStudioRenderResourceClass>
FORCEINLINE void Studio_BeginReleaseResourceSafe( CRefPtr<TStudioRenderResourceClass> pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioUpdateResourceCmd, CRefPtr<TStudioRenderResourceClass>, pResource, pResource,
										{
											pResource->ReleaseResource();
										} );
}

/*
==================
CStudioGlobalRenderResources::GetResourceList
==================
*/
FORCEINLINE eastl::unordered_set<IStudioRenderResource*>& CStudioGlobalRenderResources::GetResourceList()
{
	static eastl::unordered_set<IStudioRenderResource*> s_studioGlobalResources;
	return s_studioGlobalResources;
}

/*
==================
CStudioGlobalRenderResources::AddResource
==================
*/
FORCEINLINE void CStudioGlobalRenderResources::AddResource( IStudioRenderResource* pResource )
{
	CScopeLock scopeLock( GetThreadMutex() );
	GetResourceList().insert( pResource );
}

/*
==================
CStudioGlobalRenderResources::RemoveResource
==================
*/
FORCEINLINE void CStudioGlobalRenderResources::RemoveResource( IStudioRenderResource* pResource )
{
	CScopeLock scopeLock( GetThreadMutex() );
	GetResourceList().erase( pResource );
}

/*
==================
CStudioGlobalRenderResources::InitResources
==================
*/
FORCEINLINE void CStudioGlobalRenderResources::InitResources()
{
	CScopeLock									scopeLock( GetThreadMutex() );
	eastl::unordered_set<IStudioRenderResource*>& globalResources = GetResourceList();
	for ( auto it = globalResources.begin(), itEnd = globalResources.end(); it != itEnd; ++it )
	{
		Studio_BeginInitResource( *it );
	}
}

/*
==================
CStudioGlobalRenderResources::ReleaseResources
==================
*/
FORCEINLINE void CStudioGlobalRenderResources::ReleaseResources()
{
	CScopeLock									scopeLock( GetThreadMutex() );
	eastl::unordered_set<IStudioRenderResource*>& globalResources = GetResourceList();
	for ( auto it = globalResources.begin(), itEnd = globalResources.end(); it != itEnd; ++it )
	{
		Studio_BeginReleaseResource( *it );
	}
}

/*
==================
CStudioGlobalRenderResources::GetThreadMutex
==================
*/
FORCEINLINE CThreadMutex& CStudioGlobalRenderResources::GetThreadMutex()
{
	static CThreadMutex s_globalResourcesMutex;
	return s_globalResourcesMutex;
}

/*
==================
CStudioRenderResource::InitResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void CStudioRenderResource<TBaseClass, bGlobal>::InitResource()
{
	if ( bInitedResource )
	{
		return;
	}

	Assert( g_pStudioAPI );
	InitStudioAPI();
	bInitedResource = true;
}

/*
==================
CStudioRenderResource::ReleaseResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void CStudioRenderResource<TBaseClass, bGlobal>::ReleaseResource()
{
	if ( !bInitedResource )
	{
		return;
	}

	Assert( g_pStudioAPI );
	ReleaseStudioAPI();
	bInitedResource = false;
}

/*
==================
CStudioRenderResource::UpdateResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void CStudioRenderResource<TBaseClass, bGlobal>::UpdateResource()
{
	if ( !bInitedResource )
	{
		InitResource();
	}
	else
	{
		UpdateStudioAPI();
	}
}

/*
==================
CStudioRenderResource::IsInitedResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
bool CStudioRenderResource<TBaseClass, bGlobal>::IsInitedResource() const
{
	return bInitedResource;
}

/*
==================
CStudioRenderResource::InitStudioAPI
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void CStudioRenderResource<TBaseClass, bGlobal>::InitStudioAPI()
{
}

/*
==================
CStudioRenderResource::ReleaseStudioAPI
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void CStudioRenderResource<TBaseClass, bGlobal>::ReleaseStudioAPI()
{
}

/*
==================
CStudioRenderResource::UpdateStudioAPI
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void CStudioRenderResource<TBaseClass, bGlobal>::UpdateStudioAPI()
{
	ReleaseStudioAPI();
	InitStudioAPI();
}
