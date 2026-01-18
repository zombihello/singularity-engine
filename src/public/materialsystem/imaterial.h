#pragma once
#include "stdlib/refcount.h"
#include "studiorender/studio_renderpasstypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdList;
class IMaterialVar;
class IShader;

//-----------------------------------------------------------------------------
// Material interface
//-----------------------------------------------------------------------------
class IMaterial : public IRefCounted
{
public:
	virtual ~IMaterial() {}

	// Place barriers into a list
	virtual void		  R_Barrier( IStudioAPICmdList* pStudioAPICmdList )												  = 0;
	virtual void		  R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType ) = 0;
	virtual void		  SetShader( const char* pShaderName )															  = 0;
	virtual IMaterialVar* FindVar( const char* pName ) const															  = 0;

	virtual uint32		   GetNumVars() const	 = 0;
	virtual IMaterialVar** GetVars() const		 = 0;
	virtual const char*   GetShaderName() const = 0;
	virtual IShader*	   GetShader() const	 = 0;
};