#pragma once
#include "studiorender/studio_renderpasstypes.h"
#include "resourcesystem/iresource.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdList;
class IMaterialVar;
class IShader;
class IShaderContextData;

//-----------------------------------------------------------------------------
// Material interface
//-----------------------------------------------------------------------------
class IMaterial : public IResourceData
{
public:
	virtual ~IMaterial() {}

	// Place barriers into a list
	virtual void		  R_Barrier( IStudioAPICmdList* pStudioAPICmdList )												  = 0;
	virtual void		  R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType ) = 0;
	virtual void		  SetShader( const char* pShaderName )															  = 0;
	virtual IMaterialVar* FindVar( const char* pName ) const															  = 0;

	virtual uint32				GetNumVars() const			 = 0;
	virtual IMaterialVar**		GetVars() const				 = 0;
	virtual const char*			GetShaderName() const		 = 0;
	virtual IShader*			GetShader() const			 = 0;
	virtual IShaderContextData* GetShaderContextData() const = 0;
};

DECLARE_RESOURCE_TYPE( IMaterial, RESOURCE_TYPE_MATERIAL );
