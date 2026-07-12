#pragma once
#include "modelsystem/vertexfactory_base.h"

//-----------------------------------------------------------------------------
// Vertex declaration for modelStaticVertex_t
//-----------------------------------------------------------------------------
class CVertexDeclarationStatic : public CVertexDeclarationBase
{
private:
	// IStudioRenderResource interface
	void InitStudioAPI() override;
};

//-----------------------------------------------------------------------------
// Vertex factory for modelStaticVertex_t
//-----------------------------------------------------------------------------
class CVertexFactoryStatic : public CVertexFactoryBase
{
public:
	// IVertexFactory interface
	virtual modelVertexType_t			 GetVertexType() const override;
	virtual IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration() const override;
};

extern CVertexDeclarationStatic g_vertexDeclarationStatic;
