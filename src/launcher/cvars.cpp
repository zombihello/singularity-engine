#include "tier0/icommandline.h"
#include "appframework/iwindow.h"
#include "launcher/cvars.h"

//-----------------------------------------------------------------------------
// CVars
//-----------------------------------------------------------------------------
CCVar window_displayId( "window_displayId", "-1", "Window display id (-1: Not specified)", CVAR_FLAG_ARCHIVE );
CCVar window_width( "window_width", "-1", "Window width (-1: Use display size)", CVAR_FLAG_ARCHIVE );
CCVar window_height( "window_height", "-1", "Window height (-1: Use display size)", CVAR_FLAG_ARCHIVE );
CCVar window_refreshRate( "window_refreshRate", "0", "Window refresh rate (0: Not specified)", CVAR_FLAG_ARCHIVE );
CCVar window_mode( "window_mode", "2", "Window mode (0: Hidden, 1: Windowed, 2: Borderless Fullscreen, 3: Exclusive Fullscreen)", CVAR_FLAG_ARCHIVE );
