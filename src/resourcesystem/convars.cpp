#include "pch_resourcesystem.h"
#include "resourcesystem/convars.h"

//-----------------------------------------------------------------------------
// CVars
//-----------------------------------------------------------------------------
CCVar resSys_unusedFrameThreshold( "resSys_unusedFrameThreshold", "300", "Number of frames before an unused resource will be uncached", CVAR_FLAG_ARCHIVE );
