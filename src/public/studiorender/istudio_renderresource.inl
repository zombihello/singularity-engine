#ifndef ISTUDIO_RENDERRESOURCE_INL
#define ISTUDIO_RENDERRESOURCE_INL

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
FORCEINLINE void Studio_BeginInitResourceSafe( TRefPtr<TStudioRenderResourceClass> pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioInitResourceCmd, TRefPtr<TStudioRenderResourceClass>, pResource, pResource,
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
FORCEINLINE void Studio_BeginUpdateResourceSafe( TRefPtr<TStudioRenderResourceClass> pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioReleaseResourceCmd, TRefPtr<TStudioRenderResourceClass>, pResource, pResource,
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
FORCEINLINE void Studio_BeginReleaseResourceSafe( TRefPtr<TStudioRenderResourceClass> pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioUpdateResourceCmd, TRefPtr<TStudioRenderResourceClass>, pResource, pResource,
										{
											pResource->ReleaseResource();
										} );
}


/*
==================
CStudioGlobalRenderResources::GetResourceList
==================
*/
FORCEINLINE std::unordered_set<IStudioRenderResource*>& CStudioGlobalRenderResources::GetResourceList()
{
	static std::unordered_set<IStudioRenderResource*>	s_studioGlobalResources;
	return s_studioGlobalResources;
}

/*
==================
CStudioGlobalRenderResources::AddResource
==================
*/
FORCEINLINE void CStudioGlobalRenderResources::AddResource( IStudioRenderResource* pResource )
{
	CScopeLock		scopeLock( GetThreadMutex() );
	GetResourceList().insert( pResource );
}

/*
==================
CStudioGlobalRenderResources::RemoveResource
==================
*/
FORCEINLINE void CStudioGlobalRenderResources::RemoveResource( IStudioRenderResource* pResource )
{
	CScopeLock		scopeLock( GetThreadMutex() );
	GetResourceList().erase( pResource );
}

/*
==================
CStudioGlobalRenderResources::InitResources
==================
*/
FORCEINLINE void CStudioGlobalRenderResources::InitResources()
{
	CScopeLock										scopeLock( GetThreadMutex() );
	std::unordered_set<IStudioRenderResource*>& globalResources = GetResourceList();
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
	CScopeLock										scopeLock( GetThreadMutex() );
	std::unordered_set<IStudioRenderResource*>& globalResources = GetResourceList();
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
	static CThreadMutex		s_globalResourcesMutex;
	return s_globalResourcesMutex;
}


/*
==================
TStudioRenderResource::InitResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void TStudioRenderResource<TBaseClass, bGlobal>::InitResource()
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
TStudioRenderResource::ReleaseResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void TStudioRenderResource<TBaseClass, bGlobal>::ReleaseResource()
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
TStudioRenderResource::UpdateResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void TStudioRenderResource<TBaseClass, bGlobal>::UpdateResource()
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
TStudioRenderResource::IsInitedResource
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
bool TStudioRenderResource<TBaseClass, bGlobal>::IsInitedResource() const
{
	return bInitedResource;
}

/*
==================
TStudioRenderResource::InitStudioAPI
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void TStudioRenderResource<TBaseClass, bGlobal>::InitStudioAPI()
{}

/*
==================
TStudioRenderResource::ReleaseStudioAPI
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void TStudioRenderResource<TBaseClass, bGlobal>::ReleaseStudioAPI()
{}

/*
==================
TStudioRenderResource::UpdateStudioAPI
==================
*/
template<class TBaseClass, bool bGlobal /*= false*/>
void TStudioRenderResource<TBaseClass, bGlobal>::UpdateStudioAPI()
{
	ReleaseStudioAPI();
	InitStudioAPI();
}

#endif // !ISTUDIO_RENDERRESOURCE_INL
