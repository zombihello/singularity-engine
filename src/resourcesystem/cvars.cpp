#include "pch_resourcesystem.h"
#include "resourcesystem/cvars.h"

//-----------------------------------------------------------------------------
// CVars
//-----------------------------------------------------------------------------
CCVar res_unusedFrameThreshold( "res_unusedFrameThreshold", "300", "Number of frames before an unused resource will be uncached", CVAR_FLAG_ARCHIVE );
