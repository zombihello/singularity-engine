#pragma once

/*
==================
Thread_SetPriority
==================
*/
FORCEINLINE void Thread_SetPriority( threadHandle_t threadHandle, threadPriority_t threadPriority )
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
Thread_SetName
==================
*/
FORCEINLINE void Thread_SetName( threadHandle_t threadHandle, const char* pThreadName )
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
Thread_GetCurrentThreadHandle
==================
*/
FORCEINLINE threadHandle_t Thread_GetCurrentThreadHandle()
{
	return GetCurrentThread();
}

/*
==================
Thread_GetCurrentThreadId
==================
*/
FORCEINLINE threadId_t Thread_GetCurrentThreadId()
{
	return GetCurrentThreadId();
}

/*
==================
Thread_Yield
==================
*/
FORCEINLINE void Thread_Yield()
{
	Sleep( 0 );
}

/*
==================
Thread_Sleep
==================
*/
FORCEINLINE void Thread_Sleep( float seconds )
{
	Sleep( (DWORD)( seconds * 1000.0 ) );
}
