#pragma once

/*
==================
CWindowMgrSDL::GetWindow
==================
*/
FORCEINLINE IWindow* CWindowMgrSDL::GetWindow( windowId_t windowId ) const
{
	return windowId != INVALID_INDEX ? windowPool.Get( windowId ) : NULL;
}

/*
==================
CWindowMgrSDL::ChangeMainWindow
==================
*/
FORCEINLINE void CWindowMgrSDL::ChangeMainWindow( windowId_t newMainWindowId )
{
	Assert( newMainWindowId != INVALID_INDEX );
	mainWindowId = newMainWindowId;
	onChangedMainWindow.Invoke( newMainWindowId );
}

/*
==================
CWindowMgrSDL::GetOrCreateMainWindow
==================
*/
FORCEINLINE IWindow* CWindowMgrSDL::GetOrCreateMainWindow()
{
	return mainWindowId != INVALID_INDEX ? GetWindow( mainWindowId ) : CreateWindow();
}

/*
==================
CWindowMgrSDL::GetMainWindowId
==================
*/
FORCEINLINE windowId_t CWindowMgrSDL::GetMainWindowId() const
{
	return mainWindowId;
}

/*
==================
CWindowMgrSDL::OnChangedMainWindow
==================
*/
FORCEINLINE IWindowMgr::IOnChangedMainWindow* CWindowMgrSDL::OnChangedMainWindow() const
{
	return &onChangedMainWindow;
}

/*
==================
CWindowMgrSDL::OnWindowEvent
==================
*/
FORCEINLINE IWindowMgr::IOnWindowEvent* CWindowMgrSDL::OnWindowEvent() const
{
	return &onWindowEvent;
}

/*
==================
CWindowMgrSDL::OnInputEvent
==================
*/
FORCEINLINE IWindowMgr::IOnInputEvent* CWindowMgrSDL::OnInputEvent() const
{
	return &onInputEvent;
}
