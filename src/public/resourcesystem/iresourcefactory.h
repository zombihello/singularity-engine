#pragma once
#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IResource;

//-----------------------------------------------------------------------------
// Load flags and resource factory flags
//-----------------------------------------------------------------------------
enum resourceLoadFlag_t
{
	RESOURCE_LOAD_FLAG_NONE			   = 0,
	RESOURCE_LOAD_FLAG_ONLY_FIND	   = BIT( 0 ),	// Only try to find already exists a resource
	RESOURCE_LOAD_FLAG_WITHOUT_DEFAULT = BIT( 1 )	// Don't return the default resource if a resource isn't found
};

enum resourceFactoryFlag_t
{
	RESOURCE_FACTORY_FLAG_NONE	   = 0,
	RESOURCE_FACTORY_FLAG_STATIC   = BIT( 0 ),	// Specifies that the factory cannot be overridden by another one for the resource type
	RESOURCE_FACTORY_FLAG_NOT_USED = BIT( 1 )	// Specifies that the factory is no longer in use. Allows it to be redefined or unloaded even with RESOURCE_FACTORY_FLAG_STATIC flag
};

//-----------------------------------------------------------------------------
// Resource factory interface
// Path to the resource in the file system can be without file extension
//-----------------------------------------------------------------------------
class IResourceFactory
{
public:
	virtual TRefPtr<IRefCounted> CreateProceduralResource() const													  = 0;
	virtual TRefPtr<IRefCounted> LoadResource( const char* pPath, uint32 loadFlags = RESOURCE_LOAD_FLAG_NONE ) const = 0;
	virtual void				 UnloadResource( IRefCounted* pResoruce ) const										  = 0;
	virtual TRefPtr<IResource>	 GetDefaultResource() const															  = 0;
	virtual const char*		 GetFormatType() const																  = 0;
	virtual uint8				 GetFlags() const																	  = 0;
};

//-----------------------------------------------------------------------------
// Helper empty implementation of an IResourceFactory
//-----------------------------------------------------------------------------
template<class TBaseClass, uint8 defaultFlags = RESOURCE_FACTORY_FLAG_NONE>
class CBaseResourceFactory : public TBaseClass
{
public:
	// IResourceFactory interface
	virtual uint8 GetFlags() const override { return flags; }

	CBaseResourceFactory()
		: flags( GetDefaultFlags() )
	{
	}

protected:
	FORCEINLINE static uint8 GetDefaultFlags() { return defaultFlags; }

	uint8 flags;
};