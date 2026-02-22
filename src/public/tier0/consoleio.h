#pragma once
#include "tier0/tier0_api.h"
#include "tier0/types.h"
#include "tier1/math/color.h"

//-----------------------------------------------------------------------------
// Console API helpers
//-----------------------------------------------------------------------------
struct consoleColorContext_t
{
	uint16 initialColor;
	uint16 lastColor;
	uint16 badColor;
	uint16 backgroundColor;
};

// Function to attach a console for I/O to a GUI application in a reasonably smart fashion
TIER0_INTERFACE void Sys_SetupConsoleIO();

// Functions to work with colors in a console
TIER0_INTERFACE void   Sys_InitConsoleColorContext( consoleColorContext_t& context );
TIER0_INTERFACE uint16 Sys_SetConsoleColor( consoleColorContext_t& context, const CColor& color );
TIER0_INTERFACE void   Sys_RestoreConsoleColor( consoleColorContext_t& context, uint16 prevColor );
