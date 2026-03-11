#pragma once
#include "tier0/tier0.h"

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
void		   Sys_SetThreadPriority( threadHandle_t threadHandle, threadPriority_t threadPriority );
void		   Sys_SetThreadName( threadHandle_t threadHandle, const char* pThreadName );
threadHandle_t Sys_GetCurrentThreadHandle();
threadId_t	   Sys_GetCurrentThreadId();
void		   Sys_Yield();
void		   Sys_Sleep( float seconds );

//-----------------------------------------------------------------------------
// Thread base class
//-----------------------------------------------------------------------------
// A runnable object is an object that is "run" on an arbitrary thread
class CRunnableObject
{
public:
	virtual ~CRunnableObject() {}

	virtual bool   Init();
	virtual uint32 Run() = 0;
	virtual void   Stop();
	virtual void   Exit();
};

class CThreadBase
{
public:
	CThreadBase();
	virtual ~CThreadBase() {}

	virtual bool Start( CRunnableObject* pRunnableObject, uint32 stackSize = 0 ) = 0;
	virtual void Stop( bool bShouldWait = false, int32 exitCode = 0 )			 = 0;
	virtual void Suspend( bool bShouldPause = true )							 = 0;
	virtual void WaitForCompletion()											 = 0;

	void SetName( const char* pName );
	void SetPriority( threadPriority_t priority );

	bool			 IsAlive() const;
	const char*		 GetName() const;
	threadPriority_t GetPriority() const;
	threadHandle_t	 GetHandle() const;
	threadId_t		 GetId() const;
	int32			 GetExitCode() const;

protected:
	int32			 exitCode;
	threadId_t		 id;
	threadHandle_t	 handle;
	CRunnableObject* pRunnableObject;
	threadPriority_t priority;
	char			 name[64];
};

// Include platform specific implementation of interfaces
#if PLATFORM_WINDOWS
	#include "tier1/platforms/windows/win_threading.h"

typedef CThreadMutexWindows		CThreadMutex;
typedef CThreadEventWindows		CThreadEvent;
typedef CThreadSemaphoreWindows CThreadSemaphore;
typedef CThreadWindows			CThread;
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS

//-----------------------------------------------------------------------------
// This is a utility class that handles scope level locking
//-----------------------------------------------------------------------------
class CScopeLock : public CNonCopyable
{
public:
	CScopeLock( CThreadMutex& mutex );
	~CScopeLock();

private:
	CThreadMutex& syncObject;
};

#include "tier1/threading.inl"
