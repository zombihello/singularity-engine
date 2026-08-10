#pragma once
#include "modelsystem/vertexfactory_base.h"
#include "modelsystem/ivertexfactory_simple.h"

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
class CVertexFactorySimple : public CVertexFactoryBase<IVertexFactorySimple>
{
public:
	// IVertexFactory interface
	virtual IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration() const override;

	CVertexFactorySimple( const char* pDebugName = "" );
};

DECLARE_VERTEX_FACTORY( CVertexDeclarationSimple, IVertexFactorySimple );
extern CVertexDeclarationSimple g_vertexDeclarationSimple;
