
/**
 * @file
 * @addtogroup core core
 */

#ifndef THREADING_H
#define THREADING_H

#include "core/core.h"
#include "core/debug.h"

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
// Atomic operations
//-----------------------------------------------------------------------------
int32 Sys_InterlockedIncrement( volatile int32* pValue );
int32 Sys_InterlockedDecrement( volatile int32* pValue );
int32 Sys_InterlockedAdd( volatile int32* pValue, int32 amount );
int32 Sys_InterlockedExchange( volatile int32* pValue, int32 exchange );
int64 Sys_InterlockedExchange64( volatile int64* pValue, int64 exchange );
int32 Sys_InterlockedCompareExchange( volatile int32* pDest, int32 exchange, int32 comperand );
int64 Sys_InterlockedCompareExchange64( volatile int64* pDest, int64 exchange, int64 comperand );
void* Sys_InterlockedCompareExchangePointer( void** pDest, void* pExchange, void* pComperand );
int32 Sys_InterlockedOr( volatile int32* pDest, int32 value );
threadHandle_t Sys_GetCurrentThreadHandle();
threadId_t Sys_GetCurrentThreadId();
void Sys_Yield();
void Sys_Sleep( float seconds );

CORE_INTERFACE void Sys_InitMainThread();
CORE_INTERFACE bool Sys_IsInMainThread();


//-----------------------------------------------------------------------------
// Thread mutex interface
//-----------------------------------------------------------------------------
class CORE_CLASS IThreadMutex
{
public:
	virtual ~IThreadMutex() {}

	virtual void Lock() = 0;
	virtual void Lock() const = 0;
	virtual void Unlock() = 0;
	virtual void Unlock() const = 0;
};


//-----------------------------------------------------------------------------
// Thread event interface
//-----------------------------------------------------------------------------
class CORE_CLASS IThreadEvent
{
public:
	virtual ~IThreadEvent() {}

	virtual void Trigger() = 0;
	virtual void Reset() = 0;
	virtual void Pulse() = 0;
	virtual bool Wait( uint32 waitTime = -1 ) = 0;
};


//-----------------------------------------------------------------------------
// Thread semaphore interface
//-----------------------------------------------------------------------------
class CORE_CLASS IThreadSemaphore
{
public:
	virtual ~IThreadSemaphore() {}

	virtual bool Signal() = 0;
	virtual bool Post( uint32 value ) = 0;
	virtual void Wait() = 0;
	virtual bool Wait( uint32 milliseconds ) = 0;
	virtual bool TryWait() = 0;
};


//-----------------------------------------------------------------------------
// Thread interface
//-----------------------------------------------------------------------------
class CORE_CLASS IThread
{
public:
	virtual ~IThread() {}

	virtual bool Start( uint32 stackSize = 0 ) = 0;
	virtual void Stop( bool bShouldWait = false, int32 exitCode = 0 ) = 0;
	virtual void Suspend( bool bShouldPause = true ) = 0;
	virtual void WaitForCompletion() = 0;

	virtual void SetName( const achar* pName ) = 0;
	virtual void SetPriority( threadPriority_t priority ) = 0;

	virtual bool IsAlive() const = 0;
	virtual const achar* GetName() const = 0;
	virtual threadPriority_t GetPriority() const = 0;
	virtual threadHandle_t GetThreadHandle() const = 0;
	virtual threadId_t GetThreadId() const = 0;
	virtual int32 GetExitCode() const = 0;

protected:
	virtual bool ThreadInit() = 0;
	virtual uint32 ThreadRun() = 0;
	virtual void ThreadStop() = 0;
	virtual void ThreadExit() = 0;
};

// Include platform specific implementation of interfaces
#if PLATFORM_WINDOWS
	#include "core/platforms/windows/win_threading.h"

	typedef CWindowsThreadMutex			CThreadMutex;
	typedef CWindowsThreadEvent			CThreadEvent;
	typedef CWindowsThreadSemaphore		CThreadSemaphore;
	typedef CWindowsThread				CThread;
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS


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
	{}
	CScopeLock( CScopeLock& scopeLock )
		: pSyncObject( NULL )
	{}

	FORCEINLINE CScopeLock& operator=( CScopeLock& scopeLock )
	{
		return *this;
	}

	CThreadMutex*	pSyncObject;
};


#if PLATFORM_WINDOWS
	#include "core/platforms/windows/win_threading.inl"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

#endif // !THREADING_H