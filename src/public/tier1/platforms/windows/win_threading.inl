#pragma once

/*
==================
Sys_Yield
==================
*/
FORCEINLINE void Sys_Yield()
{
	Sleep( 0 );
}

/*
==================
Sys_Sleep
==================
*/
FORCEINLINE void Sys_Sleep( float seconds )
{
	Sleep( (DWORD)( seconds * 1000.0 ) );
}

/*
==================
Sys_SetThreadPriority
==================
*/
FORCEINLINE void Sys_SetThreadPriority( threadHandle_t threadHandle, threadPriority_t threadPriority )
{
	int32 winapiThreadPriority = THREAD_PRIORITY_NORMAL;
	switch ( threadPriority )
	{
	case THREAD_PRIOR_LOW: winapiThreadPriority = THREAD_PRIORITY_LOWEST; break;
	case THREAD_PRIOR_BELOW_NORMAL: winapiThreadPriority = THREAD_PRIORITY_BELOW_NORMAL; break;
	case THREAD_PRIOR_NORMAL: winapiThreadPriority = THREAD_PRIORITY_NORMAL; break;
	case THREAD_PRIOR_ABOVE_NORMAL: winapiThreadPriority = THREAD_PRIORITY_ABOVE_NORMAL; break;
	case THREAD_PRIOR_HIGH: winapiThreadPriority = THREAD_PRIORITY_HIGHEST; break;
	case THREAD_PRIOR_REALTIME: winapiThreadPriority = THREAD_PRIORITY_HIGHEST; break;
	default:
		AssertMsg( false, "Unknown thread priority 0x%X", threadPriority );
		return;
	}

	SetThreadPriority( threadHandle, winapiThreadPriority );
}

/*
==================
Sys_SetThreadName
==================
*/
FORCEINLINE void Sys_SetThreadName( threadHandle_t threadHandle, const char* pThreadName )
{
	//-----------------------------------------------------------------------------
	// Code setting the thread name for use in the debugger
	// http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
	//-----------------------------------------------------------------------------
	enum
	{
		MS_VC_EXCEPTION = 0x406D1388
	};

#pragma pack( push, 8 )
	typedef struct tagTHREADNAME_INFO
	{
		DWORD  dwType;		// Must be 0x1000.
		LPCSTR szName;		// Pointer to name (in user addr space).
		DWORD  dwThreadID;	// Thread ID (-1=caller thread).
		DWORD  dwFlags;		// Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack( pop )

	THREADNAME_INFO threadNameInfo;
	threadNameInfo.dwType	  = 0x1000;
	threadNameInfo.szName	  = pThreadName;
	threadNameInfo.dwThreadID = GetThreadId( threadHandle );
	threadNameInfo.dwFlags	  = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof( threadNameInfo ) / sizeof( ULONG_PTR ), (ULONG_PTR*)&threadNameInfo );
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

/*
==================
Sys_GetCurrentThreadHandle
==================
*/
FORCEINLINE threadHandle_t Sys_GetCurrentThreadHandle()
{
	return GetCurrentThread();
}

/*
==================
Sys_GetCurrentThreadId
==================
*/
FORCEINLINE threadId_t Sys_GetCurrentThreadId()
{
	return GetCurrentThreadId();
}

/*
==================
CThreadMutexWindows::CThreadMutexWindows
==================
*/
FORCEINLINE CThreadMutexWindows::CThreadMutexWindows()
{
	// Constructor that initializes the aggregated critical section
	// MSDN: You can improve performance significantly by choosing a small spin count for a critical section
	// of short duration. The heap manager uses a spin count of roughly 4000 for its per-heap critical sections.
	// This gives great performance and scalability in almost all worst-case scenarios
	const int32 spinCount = 4000;

	InitializeCriticalSection( &criticalSection );
	SetCriticalSectionSpinCount( &criticalSection, spinCount );
}

/*
==================
CThreadMutexWindows::~CThreadMutexWindows
==================
*/
FORCEINLINE CThreadMutexWindows::~CThreadMutexWindows()
{
	DeleteCriticalSection( &criticalSection );
}

/*
==================
CThreadMutexWindows::Lock
==================
*/
FORCEINLINE void CThreadMutexWindows::Lock()
{
	// Spin first before entering critical section, causing ring-0 transition and context switch.
	if ( TryEnterCriticalSection( &criticalSection ) == 0 )
	{
		EnterCriticalSection( &criticalSection );
	}
}

/*
==================
CThreadMutexWindows::Lock
==================
*/
FORCEINLINE void CThreadMutexWindows::Lock() const
{
	const_cast<CThreadMutexWindows*>( this )->Lock();
}

/*
==================
CThreadMutexWindows::Unlock
==================
*/
FORCEINLINE void CThreadMutexWindows::Unlock()
{
	LeaveCriticalSection( &criticalSection );
}

/*
==================
CThreadMutexWindows::Unlock
==================
*/
FORCEINLINE void CThreadMutexWindows::Unlock() const
{
	const_cast<CThreadMutexWindows*>( this )->Unlock();
}

/*
==================
CThreadEventWindows::CThreadEventWindows
==================
*/
FORCEINLINE CThreadEventWindows::CThreadEventWindows( bool bManualReset /* = false */, const char* pName /* = NULL */ )
	: handle( NULL )
{
	handle = CreateEventA( NULL, bManualReset, 0, pName );
	AssertMsg( handle, "Failed to create event (GetLastError 0x%X)", GetLastError() );
}

/*
==================
CThreadEventWindows::~CThreadEventWindows
==================
*/
FORCEINLINE CThreadEventWindows::~CThreadEventWindows()
{
	CloseHandle( handle );
}

/*
==================
CThreadEventWindows::Trigger
==================
*/
FORCEINLINE void CThreadEventWindows::Trigger()
{
	SetEvent( handle );
}

/*
==================
CThreadEventWindows::Reset
==================
*/
FORCEINLINE void CThreadEventWindows::Reset()
{
	ResetEvent( handle );
}

/*
==================
CThreadEventWindows::Pulse
==================
*/
FORCEINLINE void CThreadEventWindows::Pulse()
{
	PulseEvent( handle );
}

/*
==================
CThreadEventWindows::Wait
==================
*/
FORCEINLINE bool CThreadEventWindows::Wait( uint32 waitTime /* = -1 */ )
{
	return WaitForSingleObject( handle, waitTime ) == WAIT_OBJECT_0;
}

/*
==================
CThreadSemaphoreWindows::CThreadSemaphoreWindows
==================
*/
FORCEINLINE CThreadSemaphoreWindows::CThreadSemaphoreWindows( uint32 initialValue, uint32 maxValue, const char* pName /* = NULL */ )
	: handle( NULL )
{
	AssertMsg( maxValue > 0, "Invalid max value for semaphore" );
	AssertMsg( initialValue >= 0 && initialValue <= maxValue, "Invalid initial value for semaphore" );
	handle = CreateSemaphoreA( NULL, initialValue, maxValue, pName );
	AssertMsg( handle, "Failed to create semaphore (GetLastError 0x%X)", GetLastError() );
}

/*
==================
CThreadSemaphoreWindows::~CThreadSemaphoreWindows
==================
*/
FORCEINLINE CThreadSemaphoreWindows::~CThreadSemaphoreWindows()
{
	CloseHandle( handle );
}

/*
==================
CThreadSemaphoreWindows::Signal
==================
*/
FORCEINLINE bool CThreadSemaphoreWindows::Signal()
{
	return Post( 1 );
}

/*
==================
CThreadSemaphoreWindows::Post
==================
*/
FORCEINLINE bool CThreadSemaphoreWindows::Post( uint32 value )
{
	bool bResult = ReleaseSemaphore( handle, value, NULL );
	AssertMsg( bResult, "Failed to post semaphore (GetLastError 0x%X)", GetLastError() );
	return bResult;
}

/*
==================
CThreadSemaphoreWindows::Wait
==================
*/
FORCEINLINE void CThreadSemaphoreWindows::Wait()
{
	uint32 result = WaitForSingleObject( handle, INFINITE );
	Assert( result == WAIT_OBJECT_0 );
}

/*
==================
CThreadSemaphoreWindows::Wait
==================
*/
FORCEINLINE bool CThreadSemaphoreWindows::Wait( uint32 milliseconds )
{
	uint32 result = WaitForSingleObject( handle, milliseconds );
	Assert( result != WAIT_FAILED );
	return result != WAIT_TIMEOUT;
}

/*
==================
CThreadSemaphoreWindows::TryWait
==================
*/
FORCEINLINE bool CThreadSemaphoreWindows::TryWait()
{
	return Wait( 0 );
}
