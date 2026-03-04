#include "tier0/icommandline.h"
#include "appframework/iwindow.h"
#include "launcher/convars.h"

// ConVars
CConVar			  window_displayId( "window_displayId", "-1", "Window display id (-1: Not specified)", FCVAR_ARCHIVE );
CConVar			  window_width( "window_width", "-1", "Window width (-1: Use display size)", FCVAR_ARCHIVE );
CConVar			  window_height( "window_height", "-1", "Window height (-1: Use display size)", FCVAR_ARCHIVE );
CConVar			  window_refreshRate( "window_refreshRate", "0", "Window refresh rate (0: Not specified)", FCVAR_ARCHIVE );
CConVar			  window_mode( "window_mode", "2", "Window mode (0: Hidden, 1: Windowed, 2: Borderless Fullscreen, 3: Exclusive Fullscreen)", FCVAR_ARCHIVE );
CConVarsOverrider g_conVarsOverrider;

/*
==================
CConVarsOverrider::OverrideFromCommandLine
==================
*/
void CConVarsOverrider::OverrideFromCommandLine()
{
	// Check for windowed mode command line override
	ICommandLine* pCommandLine = CommandLine();
	if ( pCommandLine->HasParam( "windowed" ) || pCommandLine->HasParam( "window" ) )
	{
		window_mode.SetInt( WINDOW_MODE_WINDOWED );
	}
	// Check for fullscreen override
	else if ( pCommandLine->HasParam( "full" ) || pCommandLine->HasParam( "fullscreen" ) )
	{
		window_mode.SetInt( WINDOW_MODE_EXCLUSIVE_FULLSCREEN );
	}

	// Get width
	const char* pWidthParam = NULL;
	if ( pCommandLine->HasParam( "width" ) )
	{
		pWidthParam = "width";
	}
	else if ( pCommandLine->HasParam( "w" ) )
	{
		pWidthParam = "w";
	}

	// Override width
	if ( pWidthParam )
	{
		window_width.SetString( pCommandLine->GetFirstValue( pWidthParam ) );
	}

	// Get height
	const char* pHeightParam = NULL;
	if ( pCommandLine->HasParam( "height" ) )
	{
		pHeightParam = "height";
	}
	else if ( pCommandLine->HasParam( "h" ) )
	{
		pHeightParam = "h";
	}

	// Override height
	if ( pHeightParam )
	{
		window_height.SetString( pCommandLine->GetFirstValue( pHeightParam ) );
	}
}
