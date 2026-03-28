#pragma once

/*
==================
CStudioGlobalRenderResources::InitResources
==================
*/
FORCEINLINE void CStudioGlobalRenderResources::InitResources()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CScopeLock									  scopeLock( GetThreadMutex() );
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
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	CScopeLock									  scopeLock( GetThreadMutex() );
	eastl::unordered_set<IStudioRenderResource*>& globalResources = GetResourceList();
	for ( auto it = globalResources.begin(), itEnd = globalResources.end(); it != itEnd; ++it )
	{
		Studio_BeginReleaseResource( *it );
	}
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
CStudioRenderResource::CStudioRenderResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE CStudioRenderResource<TBaseClass, bGlobal>::CStudioRenderResource()
{
	if ( bGlobal )
	{
		CStudioGlobalRenderResources::AddResource( this );
	}

	numPendingRenderOps.store( 0, eastl::memory_order_release );
	bInitedResource.store( false, eastl::memory_order_release );
}

/*
==================
CStudioRenderResource::~CStudioRenderResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE CStudioRenderResource<TBaseClass, bGlobal>::~CStudioRenderResource()
{
	if ( bGlobal )
	{
		CStudioGlobalRenderResources::RemoveResource( this );
	}

	// Deleting an initialized/in-use IStudioRenderResource may cause a crash later
	AssertMsg( !IsInitedResource(), "An IStudioRenderResource was deleted without being released first!" );
	AssertMsg( numPendingRenderOps.load( eastl::memory_order_acquire ) == 0, "An IStudioRenderResource was deleted when some render ops hasn't been done!" );
}

/*
==================
CStudioRenderResource::InitResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE void CStudioRenderResource<TBaseClass, bGlobal>::InitResource()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( bInitedResource.load( eastl::memory_order_relaxed ) )
	{
		return;
	}

	Assert( g_pStudioAPI );
	InitStudioAPI();
	bInitedResource.store( true, eastl::memory_order_release );
}

/*
==================
CStudioRenderResource::ReleaseResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE void CStudioRenderResource<TBaseClass, bGlobal>::ReleaseResource()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( !bInitedResource.load( eastl::memory_order_relaxed ) )
	{
		return;
	}

	Assert( g_pStudioAPI );
	ReleaseStudioAPI();
	bInitedResource.store( false, eastl::memory_order_release );
}

/*
==================
CStudioRenderResource::UpdateResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE void CStudioRenderResource<TBaseClass, bGlobal>::UpdateResource()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	if ( !bInitedResource.load( eastl::memory_order_relaxed ) )
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
CStudioRenderResource::BeginEnqueueRenderOp
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE void CStudioRenderResource<TBaseClass, bGlobal>::BeginEnqueueRenderOp()
{
	numPendingRenderOps.fetch_add( 1, eastl::memory_order_acq_rel );
}

/*
==================
CStudioRenderResource::EndEnqueueRenderOp
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE void CStudioRenderResource<TBaseClass, bGlobal>::EndEnqueueRenderOp()
{
	uint32 prevNumPendingRenderOps = numPendingRenderOps.fetch_sub( 1, eastl::memory_order_acq_rel );
	Assert( prevNumPendingRenderOps > 0 );
}

/*
==================
CStudioRenderResource::IsNeedDeferredDestroy
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE bool CStudioRenderResource<TBaseClass, bGlobal>::IsNeedDeferredDestroy() const
{
	return bInitedResource.load( eastl::memory_order_acquire ) || numPendingRenderOps.load( eastl::memory_order_acquire ) != 0;
}

/*
==================
CStudioRenderResource::IsInitedResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE bool CStudioRenderResource<TBaseClass, bGlobal>::IsInitedResource() const
{
	return bInitedResource.load( eastl::memory_order_acquire );
}

/*
==================
CStudioRenderResource::InitStudioAPI
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE void CStudioRenderResource<TBaseClass, bGlobal>::InitStudioAPI()
{
}

/*
==================
CStudioRenderResource::ReleaseStudioAPI
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE void CStudioRenderResource<TBaseClass, bGlobal>::ReleaseStudioAPI()
{
}

/*
==================
CStudioRenderResource::UpdateStudioAPI
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
FORCEINLINE void CStudioRenderResource<TBaseClass, bGlobal>::UpdateStudioAPI()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	ReleaseStudioAPI();
	InitStudioAPI();
}

/*
==================
Studio_BeginInitResource
==================
*/
FORCEINLINE void Studio_BeginInitResource( IStudioRenderResource* pResource )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pResource );
	pResource->BeginEnqueueRenderOp();
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioInitResourceCmd, IStudioRenderResource*, pResource, pResource,
										{
											pResource->InitResource();
											pResource->EndEnqueueRenderOp();
										} );
}

/*
==================
Studio_BeginUpdateResource
==================
*/
FORCEINLINE void Studio_BeginUpdateResource( IStudioRenderResource* pResource )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pResource );
	pResource->BeginEnqueueRenderOp();
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioUpdateResourceCmd, IStudioRenderResource*, pResource, pResource,
										{
											pResource->UpdateResource();
											pResource->EndEnqueueRenderOp();
										} );
}

/*
==================
Studio_BeginReleaseResource
==================
*/
FORCEINLINE void Studio_BeginReleaseResource( IStudioRenderResource* pResource )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pResource );
	pResource->BeginEnqueueRenderOp();
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioReleaseResourceCmd, IStudioRenderResource*, pResource, pResource,
										{
											pResource->ReleaseResource();
											pResource->EndEnqueueRenderOp();
										} );
}

/*
==================
Studio_BeginDeleteResource
==================
*/
FORCEINLINE void Studio_BeginDeleteResource( IStudioRenderResource* pResource )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pResource );
	pResource->BeginEnqueueRenderOp();
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioDeleteResourceCmd, IStudioRenderResource*, pResource, pResource,
										{
											pResource->ReleaseResource();
											pResource->EndEnqueueRenderOp();
											delete pResource;
										} );
}
