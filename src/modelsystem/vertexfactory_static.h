#pragma once
#include "modelsystem/vertexfactory_base.h"
#include "modelsystem/ivertexfactory_static.h"

//-----------------------------------------------------------------------------
// Vertex declaration for modelStaticVertex_t
//-----------------------------------------------------------------------------
class CVertexDeclarationStatic : public CVertexDeclarationBase
{
public:
	enum streamSlot_t
	{
		STREAM_SLOT_VERTEX	 = 0,
		STREAM_SLOT_INSTANCE = 1
	};

private:
	// IStudioRenderResource interface
	virtual void InitStudioAPI() override;
};

//-----------------------------------------------------------------------------
// Vertex factory for modelStaticVertex_t
//-----------------------------------------------------------------------------
class CVertexFactoryStatic : public CVertexFactoryBase<IVertexFactoryStatic, CVertexDeclarationStatic::STREAM_SLOT_INSTANCE>
{
public:
	// IVertexFactory interface
	virtual IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration() const override;

protected:
	// CVertexFactoryBase interface
	// Update the factory's buffer
	virtual void R_UpdateInstanceBuffer( byte* pData, const byte* pInstances, uint32 numInstances ) const override;
};

DECLARE_VERTEX_FACTORY( CVertexFactoryStatic, IVertexFactoryStatic );
extern CVertexDeclarationStatic g_vertexDeclarationStatic;
