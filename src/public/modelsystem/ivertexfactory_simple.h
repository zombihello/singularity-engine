#pragma once
#include "modelsystem/ivertexfactory.h"

//-----------------------------------------------------------------------------
// Model simple instance
//-----------------------------------------------------------------------------
struct modelSimpleInstance_t
{
};

//-----------------------------------------------------------------------------
// Vertex factory interface for modelSimpleVertex_t
//-----------------------------------------------------------------------------
class IVertexFactorySimple : public IVertexFactory
{
};

DECLARE_VERTEX_FACTORY_INTERFACE( IVertexFactorySimple, modelSimpleInstance_t, MODEL_VERTEXTYPE_SIMPLE );
