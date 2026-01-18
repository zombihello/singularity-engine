#include "pch_core.h"
#include "stdlib/filetools.h"
#include "stdlib/buildnum.h"
#include "core/version.h"
#include "core/crashdump_private.h"

#include <BugTrap.h>
#include <minidumpapiset.h>

/*
==================
CrashDump_PreErrorHandler
==================
*/
static void CrashDump_PreErrorHandler( INT_PTR pUserData )
{
}

/*
==================
CrashDump_SetupExceptionHandler
==================
*/
void CrashDump_SetupExceptionHandler()
{
	BT_InstallSehFilter();
	BT_SetActivityType( BTA_SHOWUI );
	BT_SetDialogMessage( BTDM_INTRO2,
						 "This is Singularity Engine crash reporting client. To help the development process, please\r\n"
						 "Submit Bug or save report and email it manually (button More...).\r\n"
						 "Many thanks in advance and sorry for the inconvenience." );

	BT_SetPreErrHandler( CrashDump_PreErrorHandler, NULL );
	CrashDump_SetAppInfo( crashDumpAppInfo_t{ NULL, NULL, NULL, NULL } );
	BT_SetReportFormat( BTRF_TEXT );
	BT_SetFlags( BTF_DETAILEDMODE | BTF_ATTACHREPORT );
	BT_SetDumpType( MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory | MiniDumpWithProcessThreadData | MiniDumpWithThreadInfo );
}

/*
==================
CrashDump_OnThreadRun
==================
*/
void CrashDump_OnThreadRun()
{
	BT_SetTerminate();
}

/*
==================
CrashDump_OnThreadStop
==================
*/
void CrashDump_OnThreadStop()
{
}

/*
==================
CrashDump_AddLogFile
==================
*/
void CrashDump_AddLogFile( const char* pPath )
{
	std::string absolutePath;
	S_MakeAbsolutePath( pPath, absolutePath, "", false );
	BT_AddLogFile( absolutePath.c_str() );
}

/*
==================
CrashDump_SetMessage
==================
*/
void CrashDump_SetMessage( const char* pMsg )
{
	BT_SetUserMessage( pMsg );
}

/*
==================
CrashDump_SetAppInfo
==================
*/
void CrashDump_SetAppInfo( const crashDumpAppInfo_t& appInfo )
{
	TBuildNumber<ENGINE_GOLDDATE> engineBuildNumber;
	bool						  bValidAppName		 = appInfo.pAppName && S_Strlen( appInfo.pAppName ) > 0;
	bool						  bValidAppVersion	 = appInfo.pAppVersion && S_Strlen( appInfo.pAppVersion ) > 0;
	bool						  bValidSupportEmail = appInfo.pSupportEmail && S_Strlen( appInfo.pSupportEmail ) > 0;
	bool						  bValidSupportURL	 = appInfo.pSupportURL && S_Strlen( appInfo.pSupportURL ) > 0;

	BT_SetAppName( bValidAppName ? appInfo.pAppName : "Singularity Engine" );
	BT_SetAppVersion( bValidAppVersion ? appInfo.pAppVersion : S_Sprintf( ENGINE_VERSION_STRING " build %i (" __DATE__ " " __TIME__ ")", engineBuildNumber.GetBuildNumber() ).c_str() );
	BT_SetSupportEMail( bValidSupportEmail ? appInfo.pSupportEmail : "support@brokensingularity.com" );
	BT_SetSupportURL( bValidSupportURL ? appInfo.pSupportURL : "https://brokensingularity.com/" );
}
