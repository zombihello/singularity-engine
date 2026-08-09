#pragma once
#include "modelsystem/ivertexfactory.h"

//-----------------------------------------------------------------------------
// Model static instance
//-----------------------------------------------------------------------------
struct modelStaticInstance_t
{
	matrix4x3_t localToWorld;
};

//-----------------------------------------------------------------------------
// Vertex factory interface for modelStaticVertex_t
//-----------------------------------------------------------------------------
class IVertexFactoryStatic : public IVertexFactory
{
};

DECLARE_VERTEX_FACTORY_INTERFACE( IVertexFactoryStatic, modelStaticInstance_t, MODEL_VERTEXTYPE_STATIC );
