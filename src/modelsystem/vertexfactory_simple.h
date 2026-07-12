#pragma once
#include "modelsystem/vertexfactory_base.h"

//-----------------------------------------------------------------------------
// Vertex declaration for modelSimpleVertex_t
//-----------------------------------------------------------------------------
class CVertexDeclarationSimple : public CVertexDeclarationBase
{
private:
	// IStudioRenderResource interface
	void InitStudioAPI() override;
};

//-----------------------------------------------------------------------------
// Vertex factory for modelSimpleVertex_t
//-----------------------------------------------------------------------------
class CVertexFactorySimple : public CVertexFactoryBase
{
public:
	// IVertexFactory interface
	virtual modelVertexType_t			 GetVertexType() const override;
	virtual IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration() const override;
};

extern CVertexDeclarationSimple g_vertexDeclarationSimple;
