#ifndef IMATERIAL_H
#define IMATERIAL_H

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
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) = 0;
	virtual void R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType ) = 0;
	virtual void SetShader( const achar* pShaderName ) = 0;
	virtual IMaterialVar* FindVar( const achar* pName ) const = 0;

	virtual uint32 GetNumVars() const = 0;
	virtual IMaterialVar** GetVars() const = 0;
	virtual const achar* GetShaderName() const = 0;
	virtual IShader* GetShader() const = 0;
};

#endif // !IMATERIAL_H