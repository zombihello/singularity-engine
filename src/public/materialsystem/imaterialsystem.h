#pragma once
#include "stdlib/refcount.h"
#include "appframework/iappsystem.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IMaterial;

//-----------------------------------------------------------------------------
// Material system interface
//-----------------------------------------------------------------------------
#define MATERIALSYSTEM_INTERFACE_VERSION "SMaterialSystem001"
class IMaterialSystem : public IAppSystem
{
public:
};