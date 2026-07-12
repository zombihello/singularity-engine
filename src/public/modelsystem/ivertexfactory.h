#pragma once
#include "tier1/refcount.h"
#include "studiorender/studio_renderpasstypes.h"
#include "modelsystem/modeltypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIVertexDeclaration;
class IStudioAPICmdList;

//-----------------------------------------------------------------------------
// Vertex factory interface
//-----------------------------------------------------------------------------
class IVertexFactory : public IRefCounted
{
public:
	virtual ~IVertexFactory() {}

	// Place barriers into a command list
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const										 = 0;
	virtual void R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType ) = 0;

	virtual bool						 IsIndexed() const					   = 0;
	virtual modelVertexType_t			 GetVertexType() const				   = 0;
	virtual IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration() const = 0;
};
