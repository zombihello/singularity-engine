#pragma once
#include "resourcesystem/iresource.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IMaterialVar;
class IShader;
class IShaderContextData;
struct materialVarInfo_t;

//-----------------------------------------------------------------------------
// Initial material data
//-----------------------------------------------------------------------------
struct materialInitialData_t
{
	const char*				 pShaderName;
	uint32					 numVars;
	const materialVarInfo_t* pVars;
};

//-----------------------------------------------------------------------------
// A material resource interface which is owned by the render thread
//-----------------------------------------------------------------------------
class IMaterialResource : public IRefCounted
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
	virtual ~IMaterial() {}

	virtual void		  Init( const materialInitialData_t& initialData ) = 0;
	virtual void		  Destroy()										   = 0;
	virtual IMaterialVar* FindVar( const char* pName ) const			   = 0;

	virtual uint32			   GetNumVars() const		 = 0;
	virtual IMaterialVar**	   GetVars() const			 = 0;
	virtual const char*		   GetShaderName() const	 = 0;
	virtual IShader*		   GetShader() const		 = 0;
	virtual IMaterialResource* GetStudioResource() const = 0;
};

DECLARE_RESOURCE_TYPE( IMaterial, RESOURCE_TYPE_MATERIAL );
