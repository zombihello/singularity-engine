#pragma once

/*
==================
CApplication::CApplication
==================
*/
FORCEINLINE CApplication::CApplication( const char* pCommandLine, appInstanceHandle_t hInstance /*= NULL*/ )
	: pCommandLine( pCommandLine ? pCommandLine : "" )
	, appInstance( hInstance )
{
}

/*
==================
CApplication::Run
==================
*/
FORCEINLINE int32 CApplication::Run()
{
	Init();
	int32 retVal = Main();
	Shutdown();
	return retVal;
}

/*
==================
CApplication::IsConsoleApp
==================
*/
FORCEINLINE bool CApplication::IsConsoleApp() const
{
	const appInfo_t& appInfo = GetAppInfo();
	return appInfo.appType == APPLICATION_TYPE_CONSOLE;
}

/*
==================
CApplication::IsWindowApp
==================
*/
FORCEINLINE bool CApplication::IsWindowApp() const
{
	const appInfo_t& appInfo = GetAppInfo();
	return appInfo.appType == APPLICATION_TYPE_WINDOW;
}

/*
==================
CApplication::GetAppInstance
==================
*/
FORCEINLINE appInstanceHandle_t CApplication::GetAppInstance() const
{
	return appInstance;
}
