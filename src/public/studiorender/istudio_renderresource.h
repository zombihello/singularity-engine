/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef ISTUDIO_RENDERRESOURCE_H
#define ISTUDIO_RENDERRESOURCE_H

#include <unordered_set>

#include "interfaces/interfaces.h"
#include "core/types.h"
#include "core/platform.h"
#include "core/threading.h"
#include "studiorender/istudio_rendercmd.h"
#include "studiorender/istudiorender.h"

/**
 * @ingroup studiorender
 * @brief A render resource interface which is owned by the render thread
 */
class IStudioRenderResource
{
public:
	/**
	 * @brief Initializes the resource
	 * @warning This is only called by the render thread
	 * 
	 * Call R_BeginInitResource for init the resource
	 */
	virtual void InitResource() = 0;

	/**
	 * @brief Prepares the resource for deletion
	 * @warning This is only called by the render thread
	 * 
	 * Call R_BeginReleaseResource for release the resource
	 */
	virtual void ReleaseResource() = 0;

	/**
	 * @brief Prepares the resource for update
	 * @warning This is only called by the render thread
	 * 
	 * Call R_BeginUpdateResource for update the resource
	 */
	virtual void UpdateResource() = 0;

	/**
	 * @brief Is the resource initialized
	 * @return Return TRUE if resource is initialized, otherwise FALSE
	 */
	virtual bool IsInitedResource() const = 0;

protected:
	/**
	 * @brief Initializes the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 * 
	 * Called when the resource is initialized
	 */
	virtual void InitStudioAPI() = 0;

	/**
	 * @brief Releases the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 * 
	 * Called when the resource is released
	 */
	virtual void ReleaseStudioAPI() = 0;

	/**
	 * @brief If the resource's StudioAPI has been initialized, then release and reinitialize it. Otherwise, do nothing
	 * @warning This is only called by the render thread
	 */
	virtual void UpdateStudioAPI() = 0;
};

/**
 * @ingroup studiorender
 * @brief Sends message to render thread to initialize a resource
 * @warning For use g_pStudioRender must be valid
 * 
 * @param pResource		Render resource
 */
FORCEINLINE void Studio_BeginInitResource( IStudioRenderResource* pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioInitResourceCmd, IStudioRenderResource*, pResource, pResource,
										{
											pResource->InitResource();
										} );
}

/**
 * @ingroup studiorender
 * @brief Sends message to render thread to update a resource
 * @warning For use g_pStudioRender must be valid
 * 
 * @param pResource		Render resource
 */
FORCEINLINE void Studio_BeginUpdateResource( IStudioRenderResource* pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioReleaseResourceCmd, IStudioRenderResource*, pResource, pResource,
										{
											pResource->UpdateResource();
										} );
}

/**
 * @ingroup studiorender
 * @brief Sends message to render thread to release resource
 * @warning For use g_pStudioRender must be valid
 * 
 * @param pResource		Render resource
 */
FORCEINLINE void Studio_BeginReleaseResource( IStudioRenderResource* pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CStudioUpdateResourceCmd, IStudioRenderResource*, pResource, pResource,
										{
											pResource->ReleaseResource();
										} );
}

/**
 * @ingroup studiorender
 * @brief Sends message to render thread to initialize a resource (safe version)
 * @warning For use g_pStudioRender must be valid
 * 
 * @param pResource		Render resource
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

/**
 * @ingroup studiorender
 * @brief Sends message to render thread to update a resource (safe version)
 * @warning For use g_pStudioRender must be valid
 * 
 * @param pResource		Render resource
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

/**
 * @ingroup studiorender
 * @brief Sends message to render thread to release resource (safe version)
 * @warning For use g_pStudioRender must be valid
 *
 * @param pResource		Render resource
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

/**
 * @ingroup studiorender
 * @brief Container for all global render resources in the module
 */
class CStudioGlobalRenderResources
{
public:
	/**
	 * @brief Get global resource list
	 * @return Return global resources list
	 */
	static FORCEINLINE std::unordered_set<IStudioRenderResource*>& GetResourceList()
	{
		static std::unordered_set<IStudioRenderResource*>	s_studioGlobalResources;
		return s_studioGlobalResources;
	}

	/**
	 * @brief Add a global resource into the list
	 * @param pResource		Resource to add into the list
	 */
	static FORCEINLINE void AddResource( IStudioRenderResource* pResource )
	{
		CScopeLock		scopeLock( GetThreadMutex() );
		GetResourceList().insert( pResource );
	}

	/**
	 * @brief Remove the global resource from the list
	 * @param pResource		Resource to remove from the list
	 */
	static FORCEINLINE void RemoveResource( IStudioRenderResource* pResource )
	{
		CScopeLock		scopeLock( GetThreadMutex() );
		GetResourceList().erase( pResource );
	}

	/**
	 * @brief Initialize all global render resources
	 * @warning You must call it in IAppSystem::Init to inititialize all global resources before use in your module
	 */
	static FORCEINLINE void InitResources()
	{
		CScopeLock										scopeLock( GetThreadMutex() );
		std::unordered_set<IStudioRenderResource*>&		globalResources = GetResourceList();
		for ( auto it = globalResources.begin(), itEnd = globalResources.end(); it != itEnd; ++it )
		{
			Studio_BeginInitResource( *it );
		}
	}

	/**
	 * @brief Release all global render resources
	 * @warning You must call it in IAppSystem::Shutdown to release all global resources
	 */
	static FORCEINLINE void ReleaseResources()
	{
		CScopeLock										scopeLock( GetThreadMutex() );
		std::unordered_set<IStudioRenderResource*>&		globalResources = GetResourceList();
		for ( auto it = globalResources.begin(), itEnd = globalResources.end(); it != itEnd; ++it )
		{
			Studio_BeginReleaseResource( *it );
		}
	}

private:
	/**
	 * @brief Get thread mutex for add/remove
	 * @return Return thread mutex for add/remove
	 */
	static FORCEINLINE CThreadMutex& GetThreadMutex()
	{
		static CThreadMutex		s_globalResourcesMutex;
		return s_globalResourcesMutex;
	}
};

/**
 * @ingroup studiorender
 * @brief Base class to implement a render resource
 * @warning For use g_pStudioAPI must be valid
 */
template<class TBaseClass, bool bGlobal = false>
class TStudioRenderResource : public TBaseClass
{
public:
	/**
	 * @brief Constructor
	 */
	TStudioRenderResource()
		: bInitedResource( false )
	{
		if ( bGlobal )
		{
			CStudioGlobalRenderResources::AddResource( this );
		}
	}

	/**
	 * @brief Destructor
	 */
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

	/**
	 * @brief Initializes the resource
	 * @warning This is only called by the render thread
	 *
	 * Call R_BeginInitResource for init the resource
	 */
	virtual void InitResource() override
	{
		if ( bInitedResource )
		{
			return;
		}

		Assert( g_pStudioAPI );
		InitStudioAPI();
		bInitedResource = true;
	}

	/**
	 * @brief Prepares the resource for deletion
	 * @warning This is only called by the render thread
	 *
	 * Call R_BeginReleaseResource for release the resource
	 */
	virtual void ReleaseResource() override
	{
		if ( !bInitedResource )
		{
			return;
		}

		Assert( g_pStudioAPI )
		ReleaseStudioAPI();
		bInitedResource = false;
	}

	/**
	 * @brief Prepares the resource for update
	 * @warning This is only called by the render thread
	 *
	 * Call R_BeginUpdateResource for update the resource
	 */
	virtual void UpdateResource() override
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

	/**
	 * @brief Is the resource initialized
	 * @return Return TRUE if resource is initialized, otherwise FALSE
	 */
	virtual bool IsInitedResource() const override
	{
		return bInitedResource;
	}

protected:
	/**
	 * @brief Initializes the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 *
	 * Called when the resource is initialized
	 */
	virtual void InitStudioAPI() override
	{}

	/**
	 * @brief Releases the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 *
	 * Called when the resource is released
	 */
	virtual void ReleaseStudioAPI() override
	{}

	/**
	 * @brief If the resource's StudioAPI has been initialized, then release and reinitialize it. Otherwise, do nothing
	 * @warning This is only called by the render thread
	 */
	virtual void UpdateStudioAPI() override
	{
		ReleaseStudioAPI();
		InitStudioAPI();
	}

private:
	volatile bool	bInitedResource;		/**< Is resource initialized */
};

#endif // !ISTUDIO_RENDERRESOURCE_H