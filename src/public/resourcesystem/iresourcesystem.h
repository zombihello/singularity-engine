#pragma once
#include "appframework/iappsystem.h"
#include "resourcesystem/iresourcetypemgr.h"

//-----------------------------------------------------------------------------
// Resource system interface
//-----------------------------------------------------------------------------
#define RESOURCESYSTEM_INTERFACE_VERSION "SResourceSystem001"
class IResourceSystem : public IAppSystem
{
public:
	// Functions to install/remove a resource type manager for specific type
	template<class TResourceClass>
	IResourceTypeMgr*		  InstallResourceManagerForType();
	virtual IResourceTypeMgr* InstallResourceManager( resourceType_t type ) = 0;
	template<class TResourceClass>
	void		 RemoveResourceManagerForType();
	virtual void RemoveResourceManager( resourceType_t type ) = 0;

	// Performs per-frame resource management. Handles tasks like uncache unused resources
	// and other maintenance operations
	virtual void FrameUpdate() = 0;

	// Functions to uncache all resources
	virtual void UncacheAllResources() = 0;

	// Functions to check/get a resource type manager for specific type
	template<class TResourceClass>
	bool		 HasResourceManagerForType() const;
	virtual bool HasResourceManager( resourceType_t type ) const = 0;
	template<class TResourceClass>
	IResourceTypeMgr*		  GetResourceManagerForType() const;
	virtual IResourceTypeMgr* GetResourceManager( resourceType_t type ) const = 0;
};

#include "resourcesystem/iresourcesystem.inl"
