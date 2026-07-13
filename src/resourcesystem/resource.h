#pragma once
#include <EASTL/string.h>
#include <EASTL/list.h>
#include <EASTL/vector.h>

#include "tier1/template.h"
#include "resourcesystem/iresource.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CResourceTypeMgr;

//-----------------------------------------------------------------------------
// A resource
//-----------------------------------------------------------------------------
class CResource : public CRefCounted<IResource>
{
public:
	friend CResourceTypeMgr;
	DECLARE_EVENT( COnCached, IResource* /* pResource */ );
	DECLARE_EVENT( COnUncached, IResource* /* pResource */ );

	// IResource interface
	// To bring a data in or out of memory
	virtual bool Cache() override;
	virtual void Uncache() override;

	// Mark the resource as used
	virtual void MarkUsed() override;

	// Set/clear permanent flag
	virtual void MakePermanent() override;
	virtual void ClearPermanent() override;

	// Re-collect the dependency list from the current data.
	// Call whenever the resource's references to other resources change
	virtual void RebuildDependencies() override;

	virtual bool		   HasAllFlags( uint8 flags ) const override;
	virtual bool		   HasAnyFlags( uint8 flags ) const override;
	virtual bool		   HasPermanentHolders() const override;
	virtual IResourceData* GetData() const override;
	virtual resourceType_t GetType() const override;
	virtual const char*	   GetName() const override;
	virtual const char*	   GetPath() const override;
	virtual IOnCached*	   OnCached() const override;
	virtual IOnUncached*   OnUncached() const override;

	CResource( CResourceTypeMgr* pOwner, const char* pName, resourceType_t type, uint8 flags = RESOURCE_TYPE_NONE );
	~CResource();

	void ChangeData( const char* pPath, IResourceData* pData );
	void Uncache( bool bIgnorePermanent );
	void AddFlags( uint8 flags );
	void RemoveFlags( uint8 flags );
	void MarkUsedDependencies();

private:
	// Applies/releases a permanent hold on this resource's dependencies.
	// NOTE: Set `bFromDependent` to true when called because a dependent 
	// resource itself became (or stopped being) permanent
	void ApplyPermanentHolds( bool bFromDependent );
	void ReleasePermanentHolds( bool bFromDependent );

	resourceType_t							type;
	eastl::atomic<uint8>					flags;
	eastl::atomic<bool>						bPendingMarkUsed;
	bool									bInLruList;
	bool									bHoldsApplied;
	IResourceData*							pData;
	CResourceTypeMgr*						pOwner;
	uint64									lastUsedFrame;
	eastl::atomic<uint32>					numPermanentHolders;
	eastl::vector<CRefPtr<CResource>>		dependencies;
	eastl::string							name;
	eastl::string							path;
	eastl::list<CResource*>::const_iterator lruIt;
	mutable COnCached						onCached;
	mutable COnUncached						onUncached;
};

#include "resourcesystem/resource.inl"
