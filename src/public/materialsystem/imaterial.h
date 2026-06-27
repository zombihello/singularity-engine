#pragma once
#include "resourcesystem/iresource.h"
#include "studiorender/istudio_renderresource.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdList;
class IMaterialVar;
class IShader;
class IShaderContextData;

//-----------------------------------------------------------------------------
// A material resource interface which is owned by the render thread
//-----------------------------------------------------------------------------
class IMaterialResource : public IRefCounted, public IStudioRenderResource
{
public:
	virtual ~IMaterialResource() {}

	virtual IShader*			GetShader() const	   = 0;
	virtual IShaderContextData* GetContextData() const = 0;
};

//-----------------------------------------------------------------------------
// Material interface
//-----------------------------------------------------------------------------
class IMaterial : public IResourceData
{
public:
	DECLARE_EVENT_INTERFACE( IOnStudioResourceChanged, IMaterial* /* pMaterial */ );

	virtual ~IMaterial() {}

	virtual void		  SetShader( const char* pShaderName ) = 0;
	virtual IMaterialVar* FindVar( const char* pName ) const   = 0;

	virtual uint32					  GetNumVars() const			  = 0;
	virtual IMaterialVar**			  GetVars() const				  = 0;
	virtual const char*				  GetShaderName() const			  = 0;
	virtual IShader*				  GetShader() const				  = 0;
	virtual IMaterialResource*		  GetStudioResource() const		  = 0;
	virtual IOnStudioResourceChanged* OnStudioResourceChanged() const = 0;
};

DECLARE_RESOURCE_TYPE( IMaterial, RESOURCE_TYPE_MATERIAL );
