#include "pch_tier0.h"
#include "tier1/filetools.h"
#include "tier1/buildnum.h"
#include "tier0/version.h"
#include "tier0/icrashdump_handler.h"

#include <BugTrap.h>
#include <dbghelp.h>
#include <minidumpapiset.h>

class CCrashDumpHandlerWindows : public ICrashDumpHandler
{
public:
	// ICrashDumpHandler interface
	// Initialize and shutdown the crash dump handler
	virtual void Init() override;
	virtual void Shutdown() override;

	// Functions to attach some information to a crash dump
	virtual void SetAppInfo( const crashDumpAppInfo_t& appInfo ) override;
	virtual void SetMessage( const char* pMessage ) override;
	virtual void AddLogFile( const char* pPath ) override;

	// NOTE: Those functions must be called from every thread (except IThread, there already they are uses)
	virtual void OnThreadRun() override;
	virtual void OnThreadStop() override;

private:
	static void CDECL  OnTerminate();
	static LONG WINAPI OnUnhandledExceptionHandler( EXCEPTION_POINTERS* pExceptionPointers );
};

/*
==================
CCrashDumpHandlerWindows::Init
==================
*/
void CCrashDumpHandlerWindows::Init()
{
	SetUnhandledExceptionFilter( CCrashDumpHandlerWindows::OnUnhandledExceptionHandler );
	BT_SetActivityType( BTA_SHOWUI );
	BT_SetDialogMessage( BTDM_INTRO2,
						 "This is Singularity Engine crash reporting client. To help the development process, please\r\n"
						 "Submit Bug or save report and email it manually (button More...).\r\n"
						 "Many thanks in advance and sorry for the inconvenience." );

	SetAppInfo( crashDumpAppInfo_t{ NULL, NULL, NULL, NULL } );
	BT_SetReportFormat( BTRF_TEXT );
	BT_SetFlags( BTF_DETAILEDMODE | BTF_ATTACHREPORT );
	BT_SetDumpType( MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory | MiniDumpWithProcessThreadData | MiniDumpWithThreadInfo );
}

/*
==================
CCrashDumpHandlerWindows::Shutdown
==================
*/
void CCrashDumpHandlerWindows::Shutdown()
{
	SetUnhandledExceptionFilter( NULL );
}

/*
==================
CCrashDumpHandlerWindows::SetAppInfo
==================
*/
void CCrashDumpHandlerWindows::SetAppInfo( const crashDumpAppInfo_t& appInfo )
{
	CBuildNumber<ENGINE_GOLDDATE> engineBuildNumber;
	bool						  bValidAppName		 = appInfo.pAppName && S_Strlen( appInfo.pAppName ) > 0;
	bool						  bValidAppVersion	 = appInfo.pAppVersion && S_Strlen( appInfo.pAppVersion ) > 0;
	bool						  bValidSupportEmail = appInfo.pSupportEmail && S_Strlen( appInfo.pSupportEmail ) > 0;
	bool						  bValidSupportURL	 = appInfo.pSupportURL && S_Strlen( appInfo.pSupportURL ) > 0;

	BT_SetAppName( bValidAppName ? appInfo.pAppName : "Singularity Engine" );
	BT_SetAppVersion( bValidAppVersion ? appInfo.pAppVersion : S_Sprintf( ENGINE_VERSION_STRING " build %i (" __DATE__ " " __TIME__ ")", engineBuildNumber.GetBuildNumber() ).c_str() );
	BT_SetSupportEMail( bValidSupportEmail ? appInfo.pSupportEmail : "support@brokensingularity.com" );
	BT_SetSupportURL( bValidSupportURL ? appInfo.pSupportURL : "https://brokensingularity.com/" );
}

/*
==================
CCrashDumpHandlerWindows::SetMessage
==================
*/
void CCrashDumpHandlerWindows::SetMessage( const char* pMessage )
{
	BT_SetUserMessage( pMessage );
}

/*
==================
CCrashDumpHandlerWindows::AddLogFile
==================
*/
void CCrashDumpHandlerWindows::AddLogFile( const char* pPath )
{
	BT_AddLogFile( pPath );
}

/*
==================
CCrashDumpHandlerWindows::OnThreadRun
==================
*/
void CCrashDumpHandlerWindows::OnThreadRun()
{
	set_terminate( CCrashDumpHandlerWindows::OnTerminate );
}

/*
==================
CCrashDumpHandlerWindows::OnThreadStop
==================
*/
void CCrashDumpHandlerWindows::OnThreadStop()
{
	set_terminate( abort );
}

/*
==================
CCrashDumpHandlerWindows::OnTerminate
==================
*/
void CDECL CCrashDumpHandlerWindows::OnTerminate()
{
	// Call SymCleanup before BugTrap's filter. DbgHelp may have been
	// initialized earlier and BugTrap would fail to collect the call stack
	// for a crash dump
	SymCleanup( GetCurrentProcess() );
	BT_CallCppFilter();
}

/*
==================
CCrashDumpHandlerWindows::OnUnhandledExceptionHandler
==================
*/
LONG WINAPI CCrashDumpHandlerWindows::OnUnhandledExceptionHandler( EXCEPTION_POINTERS* pExceptionPointers )
{
	// Call SymCleanup before BugTrap's filter. DbgHelp may have been
	// initialized earlier and BugTrap would fail to collect the call stack
	// for a crash dump
	SymCleanup( GetCurrentProcess() );
	return BT_SehFilter( pExceptionPointers );
}

/*
==================
CrashDumpHandler
==================
*/
ICrashDumpHandler* CrashDumpHandler()
{
	static CCrashDumpHandlerWindows s_crashDumpHandler;
	return &s_crashDumpHandler;
}
