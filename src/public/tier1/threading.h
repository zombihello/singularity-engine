#pragma once
#include "tier0/tier0.h"
#include "tier0/debug.h"

//-----------------------------------------------------------------------------
// The list of enumerated thread priorities we support
//-----------------------------------------------------------------------------
enum threadPriority_t
{
	THREAD_PRIOR_LOW,
	THREAD_PRIOR_BELOW_NORMAL,
	THREAD_PRIOR_NORMAL,
	THREAD_PRIOR_ABOVE_NORMAL,
	THREAD_PRIOR_HIGH,
	THREAD_PRIOR_REALTIME
};

//-----------------------------------------------------------------------------
// Thread functions
//-----------------------------------------------------------------------------
void		   Thread_SetPriority( threadHandle_t threadHandle, threadPriority_t threadPriority );
void		   Thread_SetName( threadHandle_t threadHandle, const char* pThreadName );
threadHandle_t Thread_GetCurrentThreadHandle();
threadId_t	   Thread_GetCurrentThreadId();
void		   Thread_Yield();
void		   Thread_Sleep( float seconds );

//-----------------------------------------------------------------------------
// Thread mutex interface
//-----------------------------------------------------------------------------
class IThreadMutex
{
public:
	virtual ~IThreadMutex() {}

	virtual void Lock()			= 0;
	virtual void Lock() const	= 0;
	virtual void Unlock()		= 0;
	virtual void Unlock() const = 0;
};

//-----------------------------------------------------------------------------
// Thread event interface
//-----------------------------------------------------------------------------
class IThreadEvent
{
public:
	virtual ~IThreadEvent() {}

	virtual void Trigger()					  = 0;
	virtual void Reset()					  = 0;
	virtual void Pulse()					  = 0;
	virtual bool Wait( uint32 waitTime = -1 ) = 0;
};

//-----------------------------------------------------------------------------
// Thread semaphore interface
//-----------------------------------------------------------------------------
class IThreadSemaphore
{
public:
	virtual ~IThreadSemaphore() {}

	virtual bool Signal()					 = 0;
	virtual bool Post( uint32 value )		 = 0;
	virtual void Wait()						 = 0;
	virtual bool Wait( uint32 milliseconds ) = 0;
	virtual bool TryWait()					 = 0;
};

//-----------------------------------------------------------------------------
// Thread interface
//-----------------------------------------------------------------------------
class IThread
{
public:
	virtual ~IThread() {}

	virtual bool Start( uint32 stackSize = 0 )						  = 0;
	virtual void Stop( bool bShouldWait = false, int32 exitCode = 0 ) = 0;
	virtual void Suspend( bool bShouldPause = true )				  = 0;
	virtual void WaitForCompletion()								  = 0;

	virtual void SetName( const char* pName )			  = 0;
	virtual void SetPriority( threadPriority_t priority ) = 0;

	virtual bool			 IsAlive() const		 = 0;
	virtual const char*		 GetName() const		 = 0;
	virtual threadPriority_t GetPriority() const	 = 0;
	virtual threadHandle_t	 GetThreadHandle() const = 0;
	virtual threadId_t		 GetThreadId() const	 = 0;
	virtual int32			 GetExitCode() const	 = 0;

protected:
	virtual bool   ThreadInit() = 0;
	virtual uint32 ThreadRun()	= 0;
	virtual void   ThreadStop() = 0;
	virtual void   ThreadExit() = 0;
};

// Include platform specific implementation of interfaces
#if PLATFORM_WINDOWS
	#include "tier1/platforms/windows/win_threading.h"

typedef CWindowsThreadMutex		CThreadMutex;
typedef CWindowsThreadEvent		CThreadEvent;
typedef CWindowsThreadSemaphore CThreadSemaphore;
typedef CWindowsThread			CThread;
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS

//-----------------------------------------------------------------------------
// This is a utility class that handles scope level locking
//-----------------------------------------------------------------------------
class CScopeLock
{
public:
	CScopeLock( CThreadMutex* pMutex )
		: pSyncObject( pMutex )
	{
		Assert( pSyncObject );
		pSyncObject->Lock();
	}
	CScopeLock( CThreadMutex& mutex )
		: pSyncObject( &mutex )
	{
		Assert( pSyncObject );
		pSyncObject->Lock();
	}
	~CScopeLock()
	{
		Assert( pSyncObject );
		pSyncObject->Unlock();
	}

private:
	CScopeLock()
		: pSyncObject( NULL )
	{
	}
	CScopeLock( CScopeLock& scopeLock )
		: pSyncObject( NULL )
	{
	}

	FORCEINLINE CScopeLock& operator=( CScopeLock& scopeLock )
	{
		return *this;
	}

	CThreadMutex* pSyncObject;
};

#if PLATFORM_WINDOWS
	#include "tier1/platforms/windows/win_threading.inl"
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS
