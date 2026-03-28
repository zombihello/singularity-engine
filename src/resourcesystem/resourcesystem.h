#pragma once
#include <EASTL/unordered_map.h>

#include "resourcesystem/iresourcesystem.h"
#include "resourcesystem/resourcetypemgr.h"

//-----------------------------------------------------------------------------
// Resource system
//-----------------------------------------------------------------------------
class CResourceSystem : public CBaseAppSystem<IResourceSystem>
{
public:
	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;
	virtual void Shutdown() override;

	// IResourceSystem interface
	// Functions to install/remove a resource type manager for specific type
	virtual IResourceTypeMgr* InstallResourceManager( resourceType_t type ) override;
	virtual void			  RemoveResourceManager( resourceType_t type ) override;

	// Performs per-frame resource management. Handles tasks like uncache unused resources
	// and other maintenance operations
	virtual void FrameUpdate() override;

	// Functions to uncache all resources
	virtual void UncacheAllResources() override;

	// Functions to check/get a resource type manager for specific type
	virtual bool			  HasResourceManager( resourceType_t type ) const override;
	virtual IResourceTypeMgr* GetResourceManager( resourceType_t type ) const override;

	CResourceSystem();
	uint64 GetFrameNumber() const;

private:
	uint64												   frameNumber;
	eastl::unordered_map<resourceType_t, CResourceTypeMgr> resourceTypeMgrDict;
};

extern CResourceSystem g_resourceSystem;
#include "resourcesystem/resourcesystem.inl"
