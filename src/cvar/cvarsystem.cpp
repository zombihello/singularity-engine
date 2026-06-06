#include "pch_cvar.h"
#include "cvar/icvarsystem.h"

//-----------------------------------------------------------------------------
// The console variable system
//-----------------------------------------------------------------------------
class CCVarSystem : public CBaseAppSystem<ICVarSystem>
{
public:
};

static CCVarSystem s_cvarSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CCVarSystem, ICVarSystem, CVARSYSTEM_INTERFACE_VERSION, s_cvarSystem );
