#include "pch_tier1.h"
#include "tier0/icrashdump_handler.h"
#include "tier1/threading.h"

/*
==================
CThreadWindows::CThreadWindows
==================
*/
CThreadWindows::CThreadWindows()
	: pThreadInitSyncEvent( NULL )
{
}

/*
==================
CThreadWindows::~CThreadWindows
==================
*/
CThreadWindows::~CThreadWindows()
{
	// Stop the thread if it is alive
	if ( IsAlive() )
	{
		Stop( true );
	}
}

/*
==================
CThreadWindows::Start
==================
*/
bool CThreadWindows::Start( CRunnableObject* pRunnableObject, uint32 stackSize /* = 0 */ )
{
	// We can't to start a thread who already started
	AssertMsg( pRunnableObject, "Runnable object must be valid" );
	if ( IsAlive() )
	{
		AssertMsg( false, "Tried to create a thread that has already been created!" );
		return false;
	}

	// Create a sync event to guarantee Init() function is called first
	pThreadInitSyncEvent			= new CThreadEventWindows( true );
	CThreadWindows::pRunnableObject = pRunnableObject;

	// Create a new thread
	handle = CreateThread( NULL, stackSize, &CThreadWindows::ThreadMain, this, 0, NULL );
	if ( !handle )
	{
		AssertMsg( false, "Failed to create thread (GetLastError 0x%x)", GetLastError() );
		return false;
	}

	// Remember thread id
	id = ::GetThreadId( handle );

	// Let the thread start up
	pThreadInitSyncEvent->Wait( INFINITE );

	// Cleanup the sync event
	delete pThreadInitSyncEvent;
	pThreadInitSyncEvent = NULL;
	return true;
}

/*
==================
CThreadWindows::Stop
==================
*/
void CThreadWindows::Stop( bool bShouldWait /* = false */, int32 exitCode /* = 0 */ )
{
	// Do nothing if the thread isn't alive
	if ( !IsAlive() )
	{
		return;
	}

	// Let the thread have a chance to stop without brute force killing
	CThreadWindows::exitCode = exitCode;
	pRunnableObject->Stop();

	// If waiting was specified, wait the amount of time. If that fails,
	// brute force kill that thread. Very bad as that might leak
	if ( bShouldWait )
	{
		// Wait indefinitely for the thread to finish
		//
		// IMPORTANT: It's not safe to just go and kill the thread with TerminateThread()
		// as it could have a mutex lock that's shared with a thread that's continuing to run,
		// which would cause that other thread to dead-lock
		WaitForSingleObject( handle, INFINITE );
	}

	// Now clean up the thread handle so we don't leak
	CloseHandle( handle );
	handle			= INVALID_THREAD_HANDLE;
	id				= INVALID_THREAD_ID;
	pRunnableObject = NULL;
	name[0]			= '\0';
}

/*
==================
CThreadWindows::Suspend
==================
*/
void CThreadWindows::Suspend( bool bShouldPause /* = true */ )
{
	// Do nothing if the thread isn't alive
	if ( !IsAlive() )
	{
		return;
	}

	// Suspend the thread if it need
	if ( bShouldPause )
	{
		SuspendThread( handle );
	}
	// Otherwise resume the one
	else
	{
		ResumeThread( handle );
	}
}

/*
==================
CThreadWindows::WaitForCompletion
==================
*/
void CThreadWindows::WaitForCompletion()
{
	if ( IsAlive() )
	{
		// Block until this thread exits
		WaitForSingleObject( handle, INFINITE );
	}
}

/*
==================
CThreadWindows::ThreadMain
==================
*/
DWORD STDCALL CThreadWindows::ThreadMain( LPVOID pData )
{
	// Notify the crash dump handler about thread startup, set thread priority and debug name
	CThreadWindows* pTheThread = (CThreadWindows*)pData;
	CrashDumpHandler()->OnThreadRun();
	Sys_SetThreadName( pTheThread->handle, pTheThread->GetName() );
	Sys_SetThreadPriority( pTheThread->handle, pTheThread->priority );
	PROFILER_THREAD( pTheThread->GetName() );

	// Initialize the thread
	pTheThread->exitCode = -1;
	Assert( pTheThread->pRunnableObject );
	bool bInitResult = pTheThread->pRunnableObject->Init();

	// Initialization has completed, release the sync event
	pTheThread->pThreadInitSyncEvent->Trigger();

	// If the thread has not been initialized, close the thread
	if ( !bInitResult )
	{
		Warning( "Tier1: Thread '%s' failed to initialize", pTheThread->GetName() );
		return pTheThread->exitCode;
	}

	// Now run the task that needs to be done
	try
	{
		pTheThread->exitCode = pTheThread->pRunnableObject->Run();
	}
	catch ( ... )
	{
	}

	// Allow any allocated resources to be cleaned up
	pTheThread->pRunnableObject->Exit();

	// Notify the crash dump handler about thread stop
	CrashDumpHandler()->OnThreadStop();

	// Clean ourselves up without waiting
	// Now clean up the thread handle so we don't leak
	CloseHandle( pTheThread->handle );
	pTheThread->handle			= INVALID_THREAD_HANDLE;
	pTheThread->id				= INVALID_THREAD_ID;
	pTheThread->pRunnableObject = NULL;
	pTheThread->name[0]			= '\0';

	// Return from the thread with the exit code
	return pTheThread->exitCode;
}
