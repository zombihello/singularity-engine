#pragma once
#include "tier0/defines.h"
#include "tier1/math/color.h"
#include "studiorender/studioapi/istudioapi_cmdlist.h"

//-----------------------------------------------------------------------------
// StudioAPI event macros
//
// Emit a colored, nestable region or a single marker into a command list - the event tree in a frame debugger
// NOTE: Expand to nothing when ENABLE_STUDIOAPI_EVENTS is off (Retail)
//-----------------------------------------------------------------------------
#if ENABLE_STUDIOAPI_EVENTS
	#define STUDIOAPI_SCOPED_EVENT( CmdList, Color, Text ) CStudioAPIEvent CONCAT( s_autogenStudioAPIEvent_, __LINE__ )( CmdList, Color, Text )
	#define STUDIOAPI_MARKER( CmdList, Color, Text )	   ( CmdList )->InsertMarker( Color, Text )
#else
	#define STUDIOAPI_SCOPED_EVENT( CmdList, Color, Text )
	#define STUDIOAPI_MARKER( CmdList, Color, Text )
#endif	// ENABLE_STUDIOAPI_EVENTS

//-----------------------------------------------------------------------------
// StudioAPI scoped event
//
// RAII helper that opens a debug event on construction and closes it on
// destruction; used by STUDIOAPI_SCOPED_EVENT
//-----------------------------------------------------------------------------
class CStudioAPIEvent
{
public:
	CStudioAPIEvent( IStudioAPICmdList* pCmdList, const CColor& color, const char* pName );
	~CStudioAPIEvent();

private:
	IStudioAPICmdList* pCmdList;
};

#include "studiorender/studioapi/studioapi_event.inl"
