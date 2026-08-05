#pragma once
#include "tier1/refcount.h"
#include "appframework/iappsystem.h"
#include "modelsystem/modeltypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IVertexFactory;

//-----------------------------------------------------------------------------
// Model system interface
//-----------------------------------------------------------------------------
#define MODELSYSTEM_INTERFACE_VERSION "SModelSystem001"
class IModelSystem : public IAppSystem
{
public:
	virtual CRefPtr<IVertexFactory> CreateVertexFactory( modelVertexType_t vertexType ) const = 0;
};
