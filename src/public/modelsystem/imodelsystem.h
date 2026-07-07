#pragma once
#include "appframework/iappsystem.h"
#include "modelsystem/modeltypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIVertexDeclaration;

//-----------------------------------------------------------------------------
// Model system interface
//-----------------------------------------------------------------------------
#define MODELSYSTEM_INTERFACE_VERSION "SModelSystem001"
class IModelSystem : public IAppSystem
{
public:
	// Get a studio api vertex declaration for a specific vertex type
	virtual IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration( modelVertexType_t vertexType ) const = 0;
};