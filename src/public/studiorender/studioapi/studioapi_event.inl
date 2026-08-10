#pragma once

/*
==================
CStudioAPIEvent::CStudioAPIEvent
==================
*/
FORCEINLINE CStudioAPIEvent::CStudioAPIEvent( IStudioAPICmdList* pCmdList, const CColor& color, const char* pName )
	: pCmdList( pCmdList )
{
	pCmdList->BeginEvent( color, pName );
}

/*
==================
CStudioAPIEvent::~CStudioAPIEvent
==================
*/
FORCEINLINE CStudioAPIEvent::~CStudioAPIEvent()
{
	pCmdList->EndEvent();
}
