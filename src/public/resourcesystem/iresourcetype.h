#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IResource;
class IResourceData;

//-----------------------------------------------------------------------------
// Engine resource types
//-----------------------------------------------------------------------------
enum resourceTypeEngine_t
{
	RESOURCE_TYPE_NONE,
	RESOURCE_TYPE_TEXTURE,
	RESOURCE_TYPE_MATERIAL,
	RESOURCE_TYPE_MODEL,
	RESOURCE_LAST_ENGINE_TYPE = RESOURCE_TYPE_MODEL
};

//-----------------------------------------------------------------------------
// Resource type. Typedeffed because we can have engine or game types
//-----------------------------------------------------------------------------
typedef uint32 resourceType_t;

//-----------------------------------------------------------------------------
// A class used to allow clients to construct and destruct specific resource
// types
//-----------------------------------------------------------------------------
class IResourceTypeFactory
{
public:
	virtual IResourceData* Create( IResource* pResource ) const = 0;
	virtual void		   Delete( IResourceData* pData ) const = 0;
};

//-----------------------------------------------------------------------------
// A class used to allow clients to load specific resource types
// from a file format
//-----------------------------------------------------------------------------
class IResourceTypeLoader
{
public:
	virtual bool		Load( const char* pPath, IResourceData* pData ) const = 0;
	virtual const char* GetFormatName() const								  = 0;
};

//-----------------------------------------------------------------------------
// Helper class to implement of a resource type factory for specific resource type
//-----------------------------------------------------------------------------
template<class TResourceDataClass>
class CResourceTypeFactory : public IResourceTypeFactory
{
public:
	// IResourceTypeFactory interface
	virtual IResourceData* Create( IResource* pResource ) const override;
	virtual void		   Delete( IResourceData* pData ) const override;
};

//-----------------------------------------------------------------------------
// Helpers used to define resource types
//-----------------------------------------------------------------------------
template<class TResourceClass>
struct resourceTypeInfo_t
{
	typedef TResourceClass class_t;
	enum
	{
		RESOURCE_TYPE = RESOURCE_TYPE_NONE
	};
};

#define DECLARE_RESOURCE_TYPE( ClassName, ResourceType ) \
	template<>                                           \
	struct resourceTypeInfo_t<ClassName>                 \
	{                                                    \
		typedef ClassName class_t;                       \
		enum                                             \
		{                                                \
			RESOURCE_TYPE = ResourceType                 \
		};                                               \
	}

#include "resourcesystem/iresourcetype.inl"
